// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTypes.generated.h"


USTRUCT(BlueprintType)
struct ZADANIETESTOWECOVER_API FVector2Int
{
	GENERATED_BODY()

public:
	FVector2Int();
	FVector2Int(int inX, int inY);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	int Y;
};

