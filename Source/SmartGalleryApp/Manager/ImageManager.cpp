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

void UImageManager::LoadImages()
{
#if PLATFORM_ANDROID
	TArray<FString> ImagePaths;
	FAndroidGalleryHelper::LoadGalleryImages();

#elif PLATFORM_DESKTOP
	// Load images from a directory
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
					OnImagesLoaded(ImagePaths);
				}
			}
		}
	}
#endif
}

void UImageManager::OnImagesLoaded(const TArray<FString>& ImagePaths) 
{
	UE_LOG(LogTemp, Log, TEXT("Received %d images from gallery"), ImagePaths.Num());
	OnImageLoadedEvent.Broadcast(ImagePaths);
}

// 파일을 UTexture2D로 변환  
UTexture2D* UImageManager::LoadTextureFromFile(const FString& FilePath) {  
   TArray<uint8> FileData;  
   if (!FFileHelper::LoadFileToArray(FileData, *FilePath)) {  
       return nullptr;  
   }  
 
   IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));  
   EImageFormat ImageFormat = EImageFormat::JPEG;  
   if (FilePath.EndsWith(".png")) {  
       ImageFormat = EImageFormat::PNG;  
   }  
 
   TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);  
   if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(FileData.GetData(), FileData.Num())) {  
       TArray<uint8> UncompressedBGRA;  
       if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA)) {  
           UTexture2D* Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight());  
           if (Texture) {  
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
	return true;
}
