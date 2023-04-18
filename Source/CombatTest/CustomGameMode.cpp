// Copyright Epic Games, Inc. All Rights Reserved.

#include "CustomGameMode.h"
#include "CustomPlayerController.h"
#include "CombatTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

ACustomGameMode::ACustomGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ACustomPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}