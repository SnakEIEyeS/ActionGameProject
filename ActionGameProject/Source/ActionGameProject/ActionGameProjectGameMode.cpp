// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ActionGameProjectGameMode.h"
#include "ActionGameProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AActionGameProjectGameMode::AActionGameProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
