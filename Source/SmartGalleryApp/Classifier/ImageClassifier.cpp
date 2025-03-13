// Fill out your copyright notice in the Description page of Project Settings.


#include "ImageClassifier.h"
#include "Engine/AssetManager.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Misc/FileHelper.h"

// Sets default values for this component's properties
UImageClassifier::UImageClassifier()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UImageClassifier::BeginPlay()
{
	Super::BeginPlay();

	// Lazy load the ONNX model data asynchronously
	UAssetManager::GetStreamableManager().RequestAsyncLoad(LazyLoadedModelData.ToSoftObjectPath());
}


// Called every frame
void UImageClassifier::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UImageClassifier::LoadModel()
{
	// Check if the model data is loaded
    if (!LazyLoadedModelData)
    {
        UE_LOG(LogTemp, Error, TEXT("The Model Data is not loaded"))
        return false;
    }

	// Get the ONNX runtime
	TWeakInterfacePtr<INNERuntimeCPU> Runtime = UE::NNE::GetRuntime<INNERuntimeCPU>(FString("NNERuntimeORTCpu"));

	if (!Runtime.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get NNERuntimeORTCpu"))
        return false;
	}

    // Create runtime model
    TSharedPtr<UE::NNE::IModelCPU> Model = Runtime->CreateModelCPU(LazyLoadedModelData.Get());

    if (!Model.IsValid())
    {
		UE_LOG(LogTemp, Error, TEXT("Failed to create model on CPU"))
        return false;
    }

    // create model instance
    ModelInstance = Model->CreateModelInstanceCPU();
    if (!ModelInstance.IsValid())
    {
		UE_LOG(LogTemp, Error, TEXT("Failed to create model instance on CPU"))
		return false;
    }

	// Set input tensor shapes: {(1, 3, 224, 224), (1, 3, 224, 224)}
    TConstArrayView<UE::NNE::FTensorDesc> InputTensorDescs = ModelInstance->GetInputTensorDescs();
    checkf(InputTensorDescs.Num() == 2, TEXT("Siamese Network have two input tensors"));
    UE::NNE::FSymbolicTensorShape SymbolicInputTensorShape = InputTensorDescs[0].GetShape();
    UE::NNE::FTensorShape InputTensorShape = UE::NNE::FTensorShape::MakeFromSymbolic(SymbolicInputTensorShape);
    TArray< UE::NNE::FTensorShape > InputTensorShapes = { InputTensorShape, InputTensorShape };
    ModelInstance->SetInputTensorShapes(InputTensorShapes);

    return true;
}

float UImageClassifier::RunModel(const FString& ImagePath1, const FString& ImagePath2)
{
	// Check if the model instance is valid
    if (!ModelInstance.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Model instance is not valid."));
        return -1.0f;
    }

    // Load Images 
	TArray<float> InputTensor1;
	TArray<float> InputTensor2;
    LoadImageToTensorData(ImagePath1, InputTensor1);
    LoadImageToTensorData(ImagePath2, InputTensor2);
    
    // Make input tensor bindings
    TArray<UE::NNE::FTensorBindingCPU> InputBindings;
    UE::NNE::FTensorBindingCPU InputBinding1;
    UE::NNE::FTensorBindingCPU InputBinding2;
    InputBinding1.Data = InputTensor1.GetData();
	InputBinding2.Data = InputTensor2.GetData();
	InputBinding1.SizeInBytes = InputTensor1.Num() * sizeof(float);
    InputBinding2.SizeInBytes = InputTensor2.Num() * sizeof(float);
	InputBindings.Add(InputBinding1);
    InputBindings.Add(InputBinding2);

    // Make output tensor bindings
    float OutputValue = 0.0f;
    TArray<UE::NNE::FTensorBindingCPU> OutputBindings;
    UE::NNE::FTensorBindingCPU OutputBinding;
    OutputBinding.Data = &OutputValue;
    OutputBinding.SizeInBytes = sizeof(float);
    OutputBindings.Add(OutputBinding);

    // Run model
    if (ModelInstance->RunSync(InputBindings, OutputBindings) != UE::NNE::IModelInstanceRunSync::ERunSyncStatus::Ok)
    {
        UE_LOG(LogTemp, Error, TEXT("Model execution failed."));
        return -1.0f;
    }

    return OutputValue;
}

void UImageClassifier::LoadImageToTensorData(const FString& ImagePath, TArray<float>& OutTensorData)
{
    // load image bytes data from the path
    TArray<uint8> ImageData;
    if (!FFileHelper::LoadFileToArray(ImageData, *ImagePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load image: %s"), *ImagePath);
        return;
    }

    // Create image wrapper
    IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
    EImageFormat DetectedFormat = ImageWrapperModule.DetectImageFormat(ImageData.GetData(), ImageData.Num());
    TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(DetectedFormat);

    if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
    {
        int32 Width = ImageWrapper->GetWidth();
        int32 Height = ImageWrapper->GetHeight();
        TArray<uint8> RawData;

        // Convert BGRA format to RGB format
        if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData))
        {
			// Load image data to tensor data with shape (3, 224, 224)
            TArray<float> ConvertedData;
            ConvertedData.SetNumUninitialized(3 * 224 * 224);

            for (int32 y = 0; y < 224; ++y)
            {
                for (int32 x = 0; x < 224; ++x)
                {
                    int32 SrcIndex = ((y * Height / 224) * Width + (x * Width / 224)) * 4;
                    int32 DstIndex = y * 224 + x;

                    ConvertedData[DstIndex] = RawData[SrcIndex + 2] / 255.0f; // R
                    ConvertedData[DstIndex + 224 * 224] = RawData[SrcIndex + 1] / 255.0f; // G
                    ConvertedData[DstIndex + 2 * 224 * 224] = RawData[SrcIndex] / 255.0f; // B
                }
            }
            OutTensorData = MoveTemp(ConvertedData);
        }
    }
}

void UImageClassifier::Classify(const FString& ImagePath, const TArray<FCategory>& Categories, float Threshold = 0.9f)
{
	if (!ModelInstance.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Model instance is not valid."));
        return;
	}

    // Run model in the background thread
    AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this, ImagePath, Categories, Threshold]()
        {
            int BestIndex = -1;
            float MaxSimilarity = 0.0f;
            for (int i = 0; i < Categories.Num(); i++)
            {
                for (auto& CategoryImagePath : Categories[i].CategoryImagePaths)
                {
					// Compare the image with the category images and get the most similar category
                    float Similarity = RunModel(ImagePath, CategoryImagePath);
                    if (Similarity > MaxSimilarity && Similarity > Threshold)
                    {
                        MaxSimilarity = Similarity;
                        BestIndex = i;
                    }
                }
            }

			// Decide the best category as a result
            FCategory ResultCategory;
            if (BestIndex != -1)
            {
				ResultCategory = Categories[BestIndex];
            }

			// Broadcast delegate with the result in the main thread
			AsyncTask(ENamedThreads::GameThread, [this, ImagePath, ResultCategory]()
				{
					this->OnClassificationCompleteEvent.Broadcast(ImagePath, ResultCategory);
				});
        });
}