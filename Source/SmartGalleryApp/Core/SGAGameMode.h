// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGAGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SMARTGALLERYAPP_API ASGAGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ASGAGameMode();

private:
	UPROPERTY(EditAnywhere, Category= "SGA")
	class UCategoryManager* CategoryManager;

	UPROPERTY(EditAnywhere, Category = "SGA")
	class UImageManager* ImageManager;

	UPROPERTY(EditAnywhere, Category = "SGA")
	class UImageClassifier* ImageClassifier;
};
