// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NNE.h"
#include "NNERuntimeCPU.h"
#include "NNEModelData.h"
#include "ImageClassifier.generated.h"

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

	UFUNCTION(BlueprintCallable)
	bool LoadModel();
	UFUNCTION(BlueprintCallable)
	float RunModel(FString ImagePath1, FString ImagePath2);

private:
	void LoadImageToTensorData(const FString& ImagePath, TArray<float>& OutTensorData);

public:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UNNEModelData> LazyLoadedModelData;
	UPROPERTY(EditAnywhere)
	int BatchSize;

private:
	TSharedPtr<UE::NNE::IModelInstanceCPU> ModelInstance;
};
