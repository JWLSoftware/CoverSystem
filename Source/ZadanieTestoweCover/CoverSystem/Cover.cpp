// Fill out your copyright notice in the Description page of Project Settings.


#include "Cover.h"

FCover::FCover() : FCover(FVector2Int(0,0))
{
	
}

FCover::FCover(FVector2Int inPosition, FVector2Int inSize)
{
	Position = inPosition;
	Size = inSize;
}