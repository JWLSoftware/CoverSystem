// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cover.h"
#include "CommonTypes.h"
#include "GameFramework/Character.h"
#include "CoverSystem.generated.h"


enum class FieldStatus : uint8
{
	Empty,
	Cover,
	Unavailable,
	Reserved,
	Character
};


UCLASS(BlueprintType)
class ZADANIETESTOWECOVER_API ACoverSystem : public AActor
{
	GENERATED_BODY()
	
public:	
	UFUNCTION(BlueprintCallable, Category = "Cover")
	void Initialize(int inBoardWidth, int inBoardLength, TArray<FCover> inCovers, TArray<ACharacter*> inCharacters);
	

	UFUNCTION(BlueprintCallable, Category = "Debug")
	bool GetBestCoverSlot(ACharacter* inSource, ACharacter* inTarget, float angle, float minSize, FVector2Int& bestSlot);

	UFUNCTION(BlueprintCallable, Category = "Debug")
	bool SetCoverSlotReservation(ACharacter* inCharacter, FVector2Int inSlot);

	UFUNCTION(BlueprintCallable, Category = "Debug")
	bool RemoveCoverSlotReservation(ACharacter* inCharacter);
protected:
	TArray<FCover> Covers;
	TArray<ACharacter*> Characters;


protected:
	// Board parameters & functions
	int BoardWidth, BoardLength;
	TArray<FieldStatus> Board;
	TMap<ACharacter*, FVector2Int> Reservations;
	TMap<ACharacter*, FVector2Int> CharacterPositions;

	void ClearBoard();
	void ApplyCovers();
	void ApplyCharacters();

	bool IsFieldEmpty(int inX, int inY);
	FieldStatus GetFieldStatus(int inX, int inY);
	void SetFieldStatus(int inX, int inY, FieldStatus inStatus);

protected:
	// Cover slots parameters and functions
	void CalculateAllCoverSlots();

	TArray<FVector2Int> CoverSlots;

	TArray<FVector2Int> GetValidatedCoverSlots(ACharacter* inSource, FVector inTarget, float angle, float minSize);
	bool GetNearestCoverSlot(FVector inSource, TArray<FVector2Int> inSlots, FVector2Int& outBestSlot);
public:
	UFUNCTION(BlueprintCallable, Category = "Debug")
	TArray<FVector2Int> DebugGetAllCoverSlots();

	UFUNCTION(BlueprintCallable, Category = "Debug")
	TArray<FVector2Int> DebugGetValidatedSlots(ACharacter* inSource, ACharacter* inTarget, float angle, float minSize);
};
