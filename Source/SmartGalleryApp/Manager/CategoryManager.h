// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CategoryManager.generated.h"

// struct to store category information
USTRUCT(BlueprintType)
struct FCategory
{
	GENERATED_BODY()
public:
	// name of category
	UPROPERTY(BlueprintReadWrite)
	FString CategoryName;
	// paths of images which represent the category
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCategoryLoadedDelegate, const TArray<FCategory>&, LoadedCategories);

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

	// Add a new category
	UFUNCTION(BlueprintCallable)
	bool AddCategory(FCategory NewCategory);

	// Add example images to a category
	UFUNCTION(BlueprintCallable)
	bool AddCategoryImage(FString CategoryName, TArray<FString> ImagePaths, FCategory& OutCategory);
	
	// Delete a category
	UFUNCTION(BlueprintCallable)
	bool DeleteCategory(FString CategoryName);
	
	// Delete an example image from a category
	UFUNCTION(BlueprintCallable)
	bool DeleteCategoryImage(FString CategoryName, FString ImagePath, FCategory& OutCategory);

private:
	// Save categories to the path, "{ProjectDir}/Category"
	bool SaveCategories();
	// Load categiries from the path, {ProjectDir}/Category
	void LoadCateogries();
	// Copy an image file to a category folder
	FString CopyImageToCategoryFolder(const FString& ImagePath, const FString& CategoryPath);

public:
	// List of categories
	UPROPERTY(BlueprintReadWrite)
	TArray<FCategory> Categories;
	UPROPERTY(BlueprintAssignable)
	FOnCategoryLoadedDelegate OnCategoryLoadedEvent;
};
