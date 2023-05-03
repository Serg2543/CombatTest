// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CustomPlayerController.generated.h"

class ACombatTestCharacter;
class ACustomGameMode;
class ACustomHUD;

UCLASS()
class COMBATTEST_API ACustomPlayerController : public APlayerController
{
	GENERATED_BODY()
	
	public:
		ACustomHUD *HUD;

		TArray<ACombatTestCharacter *> SelectedActors;

		virtual void BeginPlay() override;
		virtual void SetupInputComponent() override;

		ACustomPlayerController();

		
	protected:
		FVector LastHit;

		void SelectionPressed();
		void SelectionReleased();
		void MoveReleased();

		ACustomGameMode *GameMode;
		
		void SpawnAI(int _Team);
		void SpawnAI_0();
		void SpawnAI_1();
};
