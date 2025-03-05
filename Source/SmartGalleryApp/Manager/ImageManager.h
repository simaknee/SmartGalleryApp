// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ImageManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnImageLoadedDelegate, const TArray<FString>&, LoadedImages);

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
	void LoadImages();

	UFUNCTION(BlueprintCallable)
	class UTexture2D* LoadTextureFromFile(const FString& FilePath);

	UFUNCTION(BlueprintCallable)
	bool MoveImage(FString& ImagePath, FString& DestinationPath);

private:
	void OnImagesLoaded(const TArray<FString>& ImagePaths);
	
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnImageLoadedDelegate OnImageLoadedEvent;
		
};
