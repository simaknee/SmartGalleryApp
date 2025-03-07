// Fill out your copyright notice in the Description page of Project Settings.


#include "ImageManager.h"
#include "Engine/Texture2D.h"
#include "ImageUtils.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#if PLATFORM_ANDROID
#include "../Android/AndroidGalleryHelper.h"
#include "../Android/AndroidPermissionHelper.h"
#elif PLATFORM_DESKTOP
#include "Developer/DesktopPlatform/Public/IDesktopPlatform.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#endif
// Sets default values for this component's properties
UImageManager::UImageManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

#if PLATFORM_ANDROID
	FAndroidGalleryHelper::OnGalleryImagesLoadedCallback.BindUObject(this, &UImageManager::OnImagesLoaded);
	// FAndroidPermissionHelper::RequestStoragePermission();
#endif
	// ...
}


// Called when the game starts
void UImageManager::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void UImageManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UImageManager::LoadGallery()
{
#if PLATFORM_ANDROID
	TArray<FString> ImagePaths;
	// Call android API
	FAndroidGalleryHelper::OpenGalleryFolder();

#elif PLATFORM_DESKTOP
	// Load gallery from a directory
	if (GEngine)
	{
		if (GEngine->GameViewport)
		{
			void* ParentWindowHandle = GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle();
			IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
			if (DesktopPlatform)
			{
				//Opening the file picker!
				FString GalleryPath;
				DesktopPlatform->OpenDirectoryDialog(ParentWindowHandle, FString("Select your gallery folder"), FPaths::ProjectDir(), GalleryPath);
				if (!GalleryPath.IsEmpty())
				{
					TArray<FString> FoundFiles;
					TArray<FString> ImagePaths;
					IFileManager::Get().FindFiles(FoundFiles, *GalleryPath, TEXT("png"));
					for (const FString& ImageFile : FoundFiles)
					{
						ImagePaths.Add(GalleryPath / ImageFile);

					}
					FoundFiles.Empty();
					IFileManager::Get().FindFiles(FoundFiles, *GalleryPath, TEXT("jpg"));
					for (const FString& ImageFile : FoundFiles)
					{
						ImagePaths.Add(GalleryPath / ImageFile);

					}
					OnGalleryLoaded(ImagePaths);
				}
			}
		}
	}
#endif
}

void UImageManager::OnGalleryLoaded(const TArray<FString>& ImagePaths)
{
	UE_LOG(LogTemp, Log, TEXT("Received %d images from gallery"), ImagePaths.Num());
	OnGalleryLoadedEvent.Broadcast(ImagePaths);
}

// 파일을 UTexture2D로 변환  
UTexture2D* UImageManager::LoadTextureFromFile(const FString& FilePath) 
{  
   TArray<uint8> FileData;  
   if (!FFileHelper::LoadFileToArray(FileData, *FilePath)) 
   {  
       return nullptr;  
   }

   IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));  
   EImageFormat ImageFormat = EImageFormat::JPEG;  
   if (FilePath.EndsWith(".png")) 
   {  
       ImageFormat = EImageFormat::PNG;  
   }  

   TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);  
   if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(FileData.GetData(), FileData.Num()))
   {  
       TArray<uint8> UncompressedBGRA;  
	   // WARNING: Image taken by Samsung Android Galaxy Phone may cause error due to invalid SOS paramerters for sequential JPEG
	   if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA)) 
	   {  
           UTexture2D* Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight());  
           if (Texture) 
		   {  
               void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);  
               FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());  
               Texture->GetPlatformData()->Mips[0].BulkData.Unlock();  
               Texture->UpdateResource();  
               return Texture;  
           }  
       }  
   }  
 
   return nullptr;  
}

bool UImageManager::MoveImage(FString& ImagePath, FString& DestinationPath)
{
		
	// Move image from ImagePath to DestinationPath
#if PLATFORM_ANDROID
	if (JNIEnv* Env = FAndroidApplication::GetJavaEnv()) 
	{
		jclass Class = FAndroidApplication::FindJavaClass("com/YourCompany/SmartGalleryApp/AndroidGalleryHelper");
		if (!Class) 
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to find AndroidGalleryHelper class"));
			return false;
		}

		jmethodID MoveFileMethod = Env->GetStaticMethodID(Class, "moveImageFile", "(Ljava/lang/String;Ljava/lang/String;)Z");
		if (!MoveFileMethod) 
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to find moveImageFile method"));
			return false;
		}

		jstring JavaSrcPath = Env->NewStringUTF(TCHAR_TO_UTF8(*ImagePath));
		jstring JavaDestPath = Env->NewStringUTF(TCHAR_TO_UTF8(*DestinationPath));

		jboolean Result = Env->CallStaticBooleanMethod(Class, MoveFileMethod, JavaSrcPath, JavaDestPath);

		Env->DeleteLocalRef(JavaSrcPath);
		Env->DeleteLocalRef(JavaDestPath);
		Env->DeleteLocalRef(Class);

		if (Result) 
		{
			UE_LOG(LogTemp, Log, TEXT("Image moved successfully from %s to %s"), *ImagePath, *DestinationPath);
			return true;
		}
	}
#elif PLATFORM_DESKTOP
	if (FPaths::FileExists(ImagePath))
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		if (PlatformFile.MoveFile(*DestinationPath, *ImagePath))
		{
			UE_LOG(LogTemp, Log, TEXT("Image moved successfully"));
			return true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Image to move does not exist!"));
	}
#endif
	UE_LOG(LogTemp, Error, TEXT("Failed to move image from %s to %s"), *ImagePath, *DestinationPath);
	return false;
}
