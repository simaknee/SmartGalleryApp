#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"

#if PLATFORM_ANDROID
#include "Android/AndroidJNI.h"
#include "Android/AndroidApplication.h"
#endif

DECLARE_DELEGATE_OneParam(FOnGalleryImagesLoaded, const TArray<FString>&);

class FAndroidGalleryHelper {
public:
    static void LoadGalleryImages();
    static void OnGalleryImagesLoaded(const TArray<FString>& ImagePaths);
    static void OpenGalleryFolder();
    static FOnGalleryImagesLoaded OnGalleryImagesLoadedCallback;
};