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

	// Load categories from the file system
	LoadCateogries();
}


// Called every frame
void UCategoryManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


bool UCategoryManager::AddCategory(FCategory NewCategory)
{
	// Check if the category already exists
	for (auto& Category : Categories)
	{
		// if category already exists, add image paths to the existing category and return true
		if (Category.CategoryName == NewCategory.CategoryName)
		{
			Category.CategoryImagePaths.Append(NewCategory.CategoryImagePaths);
			return true;
		}
	}
	// Add new category
	int32 AddIndex = Categories.Add(NewCategory);

	// Save changed categories to the file system
	SaveCategories();

	return 0 <= AddIndex;
}

bool UCategoryManager::AddCategoryImage(FString CategoryName, TArray<FString> ImagePaths, FCategory& OutCategory)
{
	// Find target category to add example images
	for (auto& Category : Categories)
	{
		if (Category.CategoryName == CategoryName)
		{
			// Add example images to the category
			Category.CategoryImagePaths.Append(ImagePaths);

			// Save changed categories to the file system
			SaveCategories();

			// Return with the updated category
			OutCategory = Category;
			return true;
		}
	}
	return false;
}

bool UCategoryManager::DeleteCategory(FString CategoryName)
{
	// Find target category to delete
	for (int i=0;i< Categories.Num();i++)
	{
		if (Categories[i].CategoryName == CategoryName)
		{
			// Remove category from categories
			Categories.RemoveAt(i);

			// Save changed categories to the file system
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
	// Find target category to delete image
	for (auto& Category : Categories)
	{
		if (Category.CategoryName == CategoryName)
		{
			// Remove example image from the category
			Category.CategoryImagePaths.Remove(ImagePath);

			// Remove image file only if it is saved in project directory
			if (ImagePath.Contains(FPaths::ProjectDir()))
			{
				IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
				PlatformFile.DeleteFile(*ImagePath);
			}

			// Return with the updated category
			OutCategory = Category;
			return true;
		}
	}
	return false;
}

bool UCategoryManager::SaveCategories()
{
	// Create base category path if it does not exist
	FString BaseCategoryPath = FPaths::ProjectDir() / TEXT("Category");
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	if (!PlatformFile.DirectoryExists(*BaseCategoryPath))
	{
		PlatformFile.CreateDirectory(*BaseCategoryPath);
	}

	// Save all categories
	for (FCategory& Category : Categories)
	{
		FString CategoryPath = BaseCategoryPath / Category.CategoryName;

		// Create category directory if it does not exist
		if (!PlatformFile.DirectoryExists(*CategoryPath))
		{
			PlatformFile.CreateDirectory(*CategoryPath);
		}

		// Copy all images to the category directory
		TArray<FString> NewImagePaths;
		for (const FString& ImagePath : Category.CategoryImagePaths)
		{
			
			FString NewImagePath = CopyImageToCategoryFolder(ImagePath, CategoryPath);
			if (!NewImagePath.IsEmpty())
			{
				NewImagePaths.Add(NewImagePath);
			}
		}

		// Replace image paths with copied image paths (so that category image paths reference from project dir and remain info between sessions)
		Category.CategoryImagePaths = NewImagePaths;
	}

	return true;
}

FString UCategoryManager::CopyImageToCategoryFolder(const FString& ImagePath, const FString& CategoryPath)
{
	// Copy image file to the category directory
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	FString FileName = FPaths::GetCleanFilename(ImagePath);
	FString NewImagePath = CategoryPath / FileName;

	// If the image file already exists in the category directory, return the path
	if (PlatformFile.FileExists(*NewImagePath))
	{
		return NewImagePath;
	}
	// If the image file does not exist in the category directory, copy the image file to the category directory
	else if (PlatformFile.CopyFile(*NewImagePath, *ImagePath))
	{
		return NewImagePath;
	}
	return "";
}


bool UCategoryManager::LoadCateogries()
{
	FString BaseCategoryPath = FPaths::ProjectDir() / TEXT("Category");
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*BaseCategoryPath))
	{
		PlatformFile.CreateDirectory(*BaseCategoryPath);
	}

	// Get all directories in the base category path
	TArray<FString> AllFileDirectories;
	PlatformFile.FindFilesRecursively(AllFileDirectories, *BaseCategoryPath, TEXT(""));
	for (const FString& FileDirectory : AllFileDirectories)
	{
		FCategory Category;

		// Get Folder Name (which is category name) from file directory
		Category.CategoryName = FPaths::GetPathLeaf(FPaths::GetPath(FileDirectory));
		
		// skip if already added category
		if (Categories.Contains(Category))
		{
			continue;
		}
		// Get all files in the category directory
		TArray<FString> ImagePaths;
		FString CategoryDirectory = BaseCategoryPath / Category.CategoryName;
		PlatformFile.FindFilesRecursively(ImagePaths, *CategoryDirectory, TEXT(""));
		Category.CategoryImagePaths = ImagePaths;
		Categories.Add(Category);
	}
	return true;

}