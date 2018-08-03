// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TPS_TutorialGameMode.h"
#include "TPS_TutorialCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATPS_TutorialGameMode::ATPS_TutorialGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
