// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectStillBlueGameMode.h"
#include "ProjectStillBlueCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectStillBlueGameMode::AProjectStillBlueGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
