// Copyright Epic Games, Inc. All Rights Reserved.

#include "prodject2_UE5GameMode.h"
#include "prodject2_UE5Character.h"
#include "UObject/ConstructorHelpers.h"

Aprodject2_UE5GameMode::Aprodject2_UE5GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
