// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CategoryManager.generated.h"

USTRUCT(BlueprintType)
struct FCategory
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite)
	FString CategoryName;
	UPROPERTY(BlueprintReadWrite)
	TArray<FString> CategoryImagePaths;

	FCategory()
	{
		CategoryName = "";
		CategoryImagePaths.Empty();
	}

	bool operator==(const FCategory& Other) const
	{
		return CategoryName == Other.CategoryName;
	}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SMARTGALLERYAPP_API UCategoryManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCategoryManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	bool AddCategory(FCategory NewCategory);
	UFUNCTION(BlueprintCallable)
	bool AddCategoryImage(FString CategoryName, TArray<FString> ImagePaths, FCategory& OutCategory);
	UFUNCTION(BlueprintCallable)
	bool DeleteCategory(FString CategoryName);
	UFUNCTION(BlueprintCallable)
	bool DeleteCategoryImage(FString CategoryName, FString ImagePath, FCategory& OutCategory);

private:

public:
	UPROPERTY(BlueprintReadWrite)
	TArray<FCategory> Categories;
		
};
