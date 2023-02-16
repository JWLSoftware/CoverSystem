// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonTypes.h"
#include "Cover.generated.h"




USTRUCT(BlueprintType)
struct ZADANIETESTOWECOVER_API FCover
{
	GENERATED_BODY()

public:	
	FCover();
	FCover(FVector2Int inPosition, FVector2Int inSize = FVector2Int(1,1));

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FVector2Int Position;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	FVector2Int Size;
};
