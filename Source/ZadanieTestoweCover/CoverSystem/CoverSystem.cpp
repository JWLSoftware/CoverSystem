// Fill out your copyright notice in the Description page of Project Settings.


#include "CoverSystem.h"

void ACoverSystem::Initialize(int inBoardWidth, int inBoardLength, TArray<FCover> inCovers, TArray<ACharacter*> inCharacters)
{
	Covers = inCovers;
	Characters = inCharacters;

	BoardWidth = inBoardWidth;
	BoardLength = inBoardLength;

	ClearBoard();
	ApplyCovers();
	ApplyCharacters();

	CalculateAllCoverSlots();
}


void ACoverSystem::ClearBoard()
{
	Reservations.Empty();
	CharacterPositions.Empty();

	Board.Empty(BoardWidth * BoardLength);

	for (int _index = 0; _index < BoardWidth * BoardLength; _index++)
		Board.Add(FieldStatus::Empty);
}

void ACoverSystem::ApplyCovers()
{
	for (auto cover : Covers)	
	{
		for(int _x = 0; _x < cover.Size.X; _x++)
			for (int _y = 0; _y < cover.Size.Y; _y++)
			{
				int _pos = (_x + cover.Position.X) + BoardWidth * (_y + cover.Position.Y);
				Board[_pos] = FieldStatus::Cover;
			}
	}
}

void ACoverSystem::ApplyCharacters()
{
	for (auto character : Characters)
	{
		auto _pos = character->GetActorLocation();

		int _x = round(_pos.X/100);
		int _y = round(_pos.Y/100);

		if (IsFieldEmpty(_x, _y))
			UE_LOG(LogTemp, Warning, TEXT("The character is spawned in an unavailable place [%d,%d]"), _x, _y);

		if (CharacterPositions.Contains(character))
		{
			auto _prevPos = CharacterPositions[character];

			// Position doesn't change 
			if (_prevPos.X == _x && _prevPos.Y == _y)
				continue;

			// Otherwise I update character position
			if (GetFieldStatus(_prevPos.X, _prevPos.Y) == FieldStatus::Character)
				SetFieldStatus(_prevPos.X, _prevPos.Y, FieldStatus::Empty);

			CharacterPositions[character] = FVector2Int(_x, _y);
			SetFieldStatus(_x, _y, FieldStatus::Character);
		}
		else
		{
			CharacterPositions.Add(character, FVector2Int(_x, _y));
			SetFieldStatus(_x, _y, FieldStatus::Character);
		}
	}
}

bool ACoverSystem::IsFieldEmpty(int inX, int inY)
{
	// TODO
	// If the field is occupied or reserved by the same character that "asks" if this field is empty, then this function should return true

	auto _state = GetFieldStatus(inX, inY);

	return _state == FieldStatus::Empty;	
}

FieldStatus ACoverSystem::GetFieldStatus(int inX, int inY)
{
	if (inX < 0 || inY < 0 || inX >= BoardWidth || inY >= BoardLength)
		return FieldStatus::Unavailable;

	return Board[inX + inY * BoardWidth];
}

void ACoverSystem::SetFieldStatus(int inX, int inY, FieldStatus inStatus)
{	
	if (inX >= 0 && inY >= 0 && inX < BoardWidth && inY < BoardLength)
		Board[inX + inY * BoardWidth] = inStatus;
}


void ACoverSystem::CalculateAllCoverSlots()
{
	CoverSlots.Empty();

	TArray<FVector2Int> tempCoverSlots;

	for (auto cover : Covers)
	{
		for (int _x = 0; _x < cover.Size.X; _x++)
		{
			// bottom cover slots
			tempCoverSlots.Add(FVector2Int(cover.Position.X + _x, cover.Position.Y - 1));

			// top cover slots			
			tempCoverSlots.Add(FVector2Int(cover.Position.X + _x, cover.Position.Y + cover.Size.Y));
		}

		for (int _y = 0; _y < cover.Size.Y; _y++)
		{
			// Left cover slots
			tempCoverSlots.Add(FVector2Int(cover.Position.X - 1, cover.Position.Y + _y));

			// Right cover slots			
			tempCoverSlots.Add(FVector2Int(cover.Position.X + cover.Size.X, cover.Position.Y + _y));
		}
	}

	for (auto coverSlot : tempCoverSlots)
	{
		if (coverSlot.X >= 0 && coverSlot.X <= BoardWidth && coverSlot.Y >= 0 && coverSlot.Y < BoardLength)
		{
			FieldStatus _state = GetFieldStatus(coverSlot.X, coverSlot.Y);
			if (_state != FieldStatus::Cover && _state != FieldStatus::Unavailable)
				CoverSlots.Add(coverSlot);
		}
	}
}

bool ACoverSystem::GetBestCoverSlot(ACharacter* inSource, ACharacter* inTarget, float angle, float minSize, FVector2Int& bestSlot)
{
	auto inTargetLocation = inTarget->GetActorLocation();
	auto inSourceLocation = inSource->GetActorLocation();
	
	TArray<FVector2Int> validatedSlots = GetValidatedCoverSlots(inSource, inTargetLocation, angle, minSize);
	
	return GetNearestCoverSlot(inSourceLocation,validatedSlots, bestSlot);
}

TArray<FVector2Int> ACoverSystem::GetValidatedCoverSlots(ACharacter* inSource, FVector inTarget, float angle, float minSize)
{
	TArray<FVector2Int> tempCoverSlots;

	float targetX = inTarget.X / 100.0f;
	float targetY = inTarget.Y / 100.0f;

	float angleRad = FMath::DegreesToRadians(angle);	

	// This function checks if in a given direction source character has cover, and target is in the given angle
	auto _checkDirection = [this, targetX, targetY, angleRad](FVector2Int inCoverSlot, FVector2Int inDirection) -> bool {
		// Check if we have cover in this direction
		FieldStatus _state = GetFieldStatus(inCoverSlot.X + inDirection.X, inCoverSlot.Y + inDirection.Y);
		if (_state != FieldStatus::Cover)
			return false; 

		// Check angle
		FVector _target(targetX - inCoverSlot.X, targetY - inCoverSlot.Y, 0);
		_target.Normalize();

		FVector _direction(inDirection.X, inDirection.Y, 0);
		_direction.Normalize();

		// Calculate angle between vectors and compare
		float _angle = FMath::Acos(_target.Dot(_direction));
		if (FMath::Abs(_angle) < angleRad / 2)
			return true;

		UE_LOG(LogTemp, Log, TEXT("Slot [%d,%d] out of angle range (%f)"), inCoverSlot.X, inCoverSlot.Y, FMath::RadiansToDegrees(_angle))
		return false;
	};

	for (auto _slot : CoverSlots)
	{
		// Check if slot is empty
		auto _state = GetFieldStatus(_slot.X, _slot.Y);
		if (_state != FieldStatus::Empty && _state != FieldStatus::Reserved) // The cover slot can be occupied by another character
		{
			UE_LOG(LogTemp,Log, TEXT("Slot [%d,%d] isn't empty"), _slot.X, _slot.Y)
			continue;
		}

		// If slot is reserved then I will check owner of the reservation
		if (_state == FieldStatus::Reserved)
		{
			if (!Reservations.Contains(inSource))
			{
				UE_LOG(LogTemp, Log, TEXT("Slot [%d,%d] slot reserved by another character"), _slot.X, _slot.Y)
				continue;
			}

			// Check if this reservation belongs to us.
			auto _sourceCharacterReservation = Reservations[inSource];
			if (_sourceCharacterReservation.X != _slot.X || _sourceCharacterReservation.Y != _slot.Y)
			{
				UE_LOG(LogTemp, Log, TEXT("Slot [%d,%d] slot reserved by an another character"), _slot.X, _slot.Y)
				continue;
			}			
		}

		// Check min distance
		float _distance = FMath::Square(targetX - _slot.X) + FMath::Square(targetY - _slot.Y);
		if (_distance < FMath::Square(minSize))	// The cover slot is too close to target
		{
			UE_LOG(LogTemp, Log, TEXT("Slot [%d,%d] is too close (distance %f)"), _slot.X, _slot.Y, _distance)
			continue;
		}

		// Check angle
		if (_checkDirection(_slot, FVector2Int(1, 0))
			|| _checkDirection(_slot, FVector2Int(-1, 0))
			|| _checkDirection(_slot, FVector2Int(0, 1))
			|| _checkDirection(_slot, FVector2Int(0, -1)))
			tempCoverSlots.Add(_slot);
	}

	return tempCoverSlots;
}

bool ACoverSystem::GetNearestCoverSlot(FVector inSource, TArray<FVector2Int> inSlots, FVector2Int& outBestSlot)
{
	// TODO - I implemeted here a simple way to calculate the distance to the nearest cover slot. However, 
	// this distance will be longer if there are any obstacles on the path. So, this function should be extended to calculate an accurate distance.

	if (inSlots.Num() == 0)
		return false;

	float sourceX = inSource.X / 100.0f;
	float sourceY = inSource.Y / 100.0f;

	float _bestdistance = TNumericLimits<float>::Max();
	for (auto _slot: inSlots)
	{
		float _distance = FMath::Square(_slot.X - sourceX) + FMath::Square(_slot.Y - sourceY);
		if (_distance < _bestdistance)
		{
			_bestdistance = _distance;
			outBestSlot = _slot;
		}
	}	

	UE_LOG(LogTemp, Log, TEXT("Slot [%d,%d] The best cover slot !!!"), outBestSlot.X, outBestSlot.Y)	
	return true;
}

bool ACoverSystem::SetCoverSlotReservation(ACharacter* inCharacter, FVector2Int inSlot)
{
	if (Reservations.Contains(inCharacter))
	{
		FVector2Int _currReservation = Reservations[inCharacter];
		if (_currReservation.X == inSlot.X && _currReservation.Y == inSlot.Y)
		{
			// No changes needed
			return true;
		}
		else
		{
			// You can't make reservation on a no-empty field
			if (GetFieldStatus(inSlot.X, inSlot.Y) != FieldStatus::Empty)
				return false;

			// Clear old reservation on the field
			if (GetFieldStatus(_currReservation.X, _currReservation.Y) == FieldStatus::Reserved)
				SetFieldStatus(_currReservation.X, _currReservation.Y, FieldStatus::Empty);

			// Set a new reservation place
			Reservations[inCharacter] = inSlot;
			SetFieldStatus(inSlot.X, inSlot.Y, FieldStatus::Reserved);			
		}
	}
	else
	{
		// You can't make reservation on a no-empty field
		if (GetFieldStatus(inSlot.X, inSlot.Y) != FieldStatus::Empty)
			return false;

		// Set reservation
		Reservations.Add(inCharacter, inSlot);
		SetFieldStatus(inSlot.X, inSlot.Y, FieldStatus::Reserved);
	}

	return true;
}

bool ACoverSystem::RemoveCoverSlotReservation(ACharacter* inCharacter)
{
	if (Reservations.Contains(inCharacter))
	{
		auto _slot = Reservations[inCharacter];

		if (GetFieldStatus(_slot.X, _slot.Y) == FieldStatus::Reserved)
			SetFieldStatus(_slot.X, _slot.Y, FieldStatus::Empty);

		Reservations.Remove(inCharacter);
		return true;
	}
	else
		return false;
}


TArray<FVector2Int> ACoverSystem::DebugGetValidatedSlots(ACharacter* inSource, ACharacter* inTarget, float angle, float minSize)
{
	auto inTargetLocation = inTarget->GetActorLocation();
	auto inSourceLocation = inSource->GetActorLocation();

	return GetValidatedCoverSlots(inSource, inTargetLocation, angle, minSize);
}

TArray<FVector2Int> ACoverSystem::DebugGetAllCoverSlots()
{
	return CoverSlots;
}