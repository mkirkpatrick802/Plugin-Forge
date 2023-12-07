// Copyright Epic Games, Inc. All Rights Reserved.

#include "MP_PluginGameMode.h"
#include "MP_PluginCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMP_PluginGameMode::AMP_PluginGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
