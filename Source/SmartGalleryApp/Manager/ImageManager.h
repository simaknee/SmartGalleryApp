// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ImageManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGalleryLoadedDelegate, const TArray<FString>&, LoadedImages);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SMARTGALLERYAPP_API UImageManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UImageManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Select a gallery folder and load all images in the gallery
	UFUNCTION(BlueprintCallable)
	void LoadGallery();

	// Select one or more images and load them
	UFUNCTION(BlueprintCallable)
	void LoadImages();

	// Load a texture from a image file
	UFUNCTION(BlueprintCallable)
	class UTexture2D* LoadTextureFromFile(const FString& FilePath);

	// Move an image file to a new location
	UFUNCTION(BlueprintCallable)
	bool MoveImage(const FString& ImagePath, const FString& DestinationPath);

private:
	// Callback function for gallery loaded
	void OnGalleryLoaded(const TArray<FString>& ImagePaths);

	// Callback function for images loaded
	void OnImagesLoaded(const TArray<FString>& ImagePaths);

public:
	// Event for gallery loaded
	UPROPERTY(BlueprintAssignable)
	FOnGalleryLoadedDelegate OnGalleryLoadedEvent;

	// Event for images loaded
	UPROPERTY(BlueprintAssignable)
	FOnGalleryLoadedDelegate OnImagesLoadedEvent;
	UPROPERTY(BlueprintReadOnly)

	// Recently loaded gallery image paths
	TArray<FString> GalleryImagePaths;
};
