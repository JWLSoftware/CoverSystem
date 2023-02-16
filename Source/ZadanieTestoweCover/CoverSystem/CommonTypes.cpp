// Fill out your copyright notice in the Description page of Project Settings.


#include "CommonTypes.h"

FVector2Int::FVector2Int() : FVector2Int(0,0)
{

}

FVector2Int::FVector2Int(int inX, int inY)
{
	X = inX;
	Y = inY;
}