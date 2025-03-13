// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NNE.h"
#include "NNERuntimeCPU.h"
#include "NNEModelData.h"
#include "../Manager/CategoryManager.h"
#include "ImageClassifier.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClassificationCompleteDelegate, const FString&, InputImagePath, const FCategory&, ResultCategory);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SMARTGALLERYAPP_API UImageClassifier : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UImageClassifier();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Load ONNX model data
	UFUNCTION(BlueprintCallable)
	bool LoadModel();

	// Run the ONNX model with two images
	UFUNCTION(BlueprintCallable)
	float RunModel(const FString& ImagePath1, const FString& ImagePath2);
	
	// Classify an image with the given categories and threshold
	UFUNCTION(BlueprintCallable)
	void Classify(const FString& ImagePath, const TArray<FCategory>& Categories, float Threshold);
	
private:
	// Load image data to tensor data
	void LoadImageToTensorData(const FString& ImagePath, TArray<float>& OutTensorData);

public:
	// ONNX model data (lazy loading)
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UNNEModelData> LazyLoadedModelData;
	// Event for classification complete
	UPROPERTY(BlueprintAssignable)
	FOnClassificationCompleteDelegate OnClassificationCompleteEvent;

private:
	// Model instance on CPU
	TSharedPtr<UE::NNE::IModelInstanceCPU> ModelInstance;
};
