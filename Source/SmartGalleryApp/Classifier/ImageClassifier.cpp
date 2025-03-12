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
	BatchSize = 4;
	// ...
}


// Called when the game starts
void UImageClassifier::BeginPlay()
{
	Super::BeginPlay();

	// ...
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
	if (LazyLoadedModelData)
	{
		TWeakInterfacePtr<INNERuntimeCPU> Runtime = UE::NNE::GetRuntime<INNERuntimeCPU>(FString("NNERuntimeORTCpu"));
		if (Runtime.IsValid())
		{
			TSharedPtr<UE::NNE::IModelCPU> Model = Runtime->CreateModelCPU(LazyLoadedModelData.Get());
			if (Model.IsValid())
			{
				ModelInstance = Model->CreateModelInstanceCPU();
				if (ModelInstance.IsValid())
				{
					TConstArrayView<UE::NNE::FTensorDesc> InputTensorDescs = ModelInstance->GetInputTensorDescs();
					checkf(InputTensorDescs.Num() == 2, TEXT("Siamese Network have two input tensors"));
                    UE::NNE::FSymbolicTensorShape SymbolicInputTensorShape = InputTensorDescs[0].GetShape();
                    UE::NNE::FTensorShape InputTensorShape = UE::NNE::FTensorShape::MakeFromSymbolic(SymbolicInputTensorShape);
                    TArray< UE::NNE::FTensorShape > InputTensorShapes = { InputTensorShape, InputTensorShape };
                    ModelInstance->SetInputTensorShapes(InputTensorShapes);
					return true;
				}
				
			}
		}
	}
	return false;
}

float UImageClassifier::RunModel(const FString& ImagePath1, const FString& ImagePath2)
{
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
    InputBindings.Add(UE::NNE::FTensorBindingCPU(InputTensor1.GetData(), InputTensor1.Num() * sizeof(float)));
    InputBindings.Add(UE::NNE::FTensorBindingCPU(InputTensor2.GetData(), InputTensor2.Num() * sizeof(float)));

    // Make output tensor bindings
    float OutputValue = 0.0f;
    TArray<UE::NNE::FTensorBindingCPU> OutputBindings;
    OutputBindings.Add(UE::NNE::FTensorBindingCPU(&OutputValue, sizeof(float)));

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

        // convert BGRA format to RGB format
        if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, RawData))
        {
			// load image data to tensor data with shape (3, 224, 224)
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

FCategory UImageClassifier::Classify(const FString& ImagePath, const TArray<FCategory>& Categories, float Threshold = 0.9f)
{
	if (!ModelInstance.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Model instance is not valid."));
		return FCategory();
	}
    int BestIndex = -1;
    float MaxSimilarity = 0.0f;
	for (int i = 0; i < Categories.Num(); i++)
	{
		for (auto& CategoryImagePath : Categories[i].CategoryImagePaths)
		{
			float Similarity = RunModel(ImagePath, CategoryImagePath);
			if (Similarity > MaxSimilarity && Similarity > Threshold)
			{
				MaxSimilarity = Similarity;
				BestIndex = i;
			}
		}
	}
	if (BestIndex != -1)
	{
		return Categories[BestIndex];
	}
	return FCategory();
}