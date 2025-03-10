// Fill out your copyright notice in the Description page of Project Settings.


#include "CategoryManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"

// Sets default values for this component's properties
UCategoryManager::UCategoryManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UCategoryManager::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UCategoryManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


bool UCategoryManager::AddCategory(FCategory NewCategory)
{
	for (auto& Category : Categories)
	{
		if (Category.CategoryName == NewCategory.CategoryName)
		{
			Category.CategoryImagePaths.Append(NewCategory.CategoryImagePaths);
			return true;
		}
	}
	int32 AddIndex = Categories.Add(NewCategory);
	return 0 <= AddIndex;
}

bool UCategoryManager::AddCategoryImage(FString CategoryName, TArray<FString> ImagePaths,FCategory& OutCategory)
{
	for (auto& Category : Categories)
	{
		if (Category.CategoryName == CategoryName)
		{
			Category.CategoryImagePaths.Append(ImagePaths);
			OutCategory = Category;
			return true;
		}
	}
	return false;
}

bool UCategoryManager::DeleteCategory(FString CategoryName)
{
	for (int i=0;i< Categories.Num();i++)
	{
		if (Categories[i].CategoryName == CategoryName)
		{
			Categories.RemoveAt(i);
			// remove category folder
			FString BaseCategoryPath = FPaths::ProjectDir() / TEXT("Category");
			IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
			FString CategoryPath = BaseCategoryPath / CategoryName;
			PlatformFile.DeleteDirectoryRecursively(*CategoryPath);
			return true;
		}
	}
	return false;
}

bool UCategoryManager::DeleteCategoryImage(FString CategoryName, FString ImagePath, FCategory& OutCategory)
{
	for (auto& Category : Categories)
	{
		if (Category.CategoryName == CategoryName)
		{
			Category.CategoryImagePaths.Remove(ImagePath);
			// remove image file only if it is project directory
			if (ImagePath.Contains(FPaths::ProjectDir()))
			{
				IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
				PlatformFile.DeleteFile(*ImagePath);
			}
			OutCategory = Category;
			return true;
		}
	}
	return false;
}


}