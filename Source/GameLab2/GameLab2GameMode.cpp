// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "GameLab2.h"
#include "GameLab2GameMode.h"
#include "GameLab2Pawn.h"

AGameLab2GameMode::AGameLab2GameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// set default pawn class to our flying pawn
	DefaultPawnClass = AGameLab2Pawn::StaticClass();
}
