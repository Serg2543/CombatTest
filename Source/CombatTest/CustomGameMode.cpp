// Copyright Epic Games, Inc. All Rights Reserved.

#include "CustomGameMode.h"
#include "UObject/ConstructorHelpers.h"

#include "CustomPlayerController.h"
//#include "CombatTestCharacter.h"

ACustomGameMode::ACustomGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ACustomPlayerController::StaticClass();
}