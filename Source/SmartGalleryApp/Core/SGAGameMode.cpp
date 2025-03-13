// Fill out your copyright notice in the Description page of Project Settings.


#include "SGAGameMode.h"
#include "../Manager/CategoryManager.h"
#include "../Manager/ImageManager.h"
#include "../Classifier/ImageClassifier.h"

ASGAGameMode::ASGAGameMode()
{
	CategoryManager = CreateDefaultSubobject<UCategoryManager>(TEXT("CategoryManager"));
	ImageManager = CreateDefaultSubobject<UImageManager>(TEXT("ImageManager"));
	ImageClassifier = CreateDefaultSubobject<UImageClassifier>(TEXT("ImageClassifier"));
}
