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

	UFUNCTION(BlueprintCallable)
	void LoadGallery();

	UFUNCTION(BlueprintCallable)
	void LoadImages();

	UFUNCTION(BlueprintCallable)
	class UTexture2D* LoadTextureFromFile(const FString& FilePath);

	UFUNCTION(BlueprintCallable)
	bool MoveImage(const FString& ImagePath, const FString& DestinationPath);

private:
	void OnGalleryLoaded(const TArray<FString>& ImagePaths);
	void OnImagesLoaded(const TArray<FString>& ImagePaths);

public:
	UPROPERTY(BlueprintAssignable)
	FOnGalleryLoadedDelegate OnGalleryLoadedEvent;
	UPROPERTY(BlueprintAssignable)
	FOnGalleryLoadedDelegate OnImagesLoadedEvent;
	UPROPERTY(BlueprintReadOnly)
	TArray<FString> GalleryImagePaths;
};
