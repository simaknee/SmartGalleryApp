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
	// Call LoadGalleryImages method from AndroidGalleryHelper java class, which select a gallery folder and load all images in the gallery.
    static void LoadGalleryImages();

	// Callback function for gallery images loaded
    static void OnGalleryImagesLoaded(const TArray<FString>& ImagePaths);

	// Call OpenGalleryFolder method from AndroidGalleryHelper java class, which select a gallery folder.
    static void OpenGalleryFolder();

	// Call OpenImagePicker method from AndroidGalleryHelper java class, which select one or more images and load them.
    static void OpenImagePicker();

	// Callback function for selected images loaded
    static void OnSelectedImagesLoaded(const TArray<FString>& ImagePaths);

	// Callback delegate for gallery images loaded
    static FOnGalleryImagesLoaded OnGalleryImagesLoadedCallback;

	// Callback delegate for selected images loaded
    static FOnGalleryImagesLoaded OnSelectedImagesLoadedCallback;
};