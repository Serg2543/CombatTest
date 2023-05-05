// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CustomPlayerController.generated.h"

class ACombatTestCharacter;
class ACustomHUD;

UCLASS()
class COMBATTEST_API ACustomPlayerController : public APlayerController
{
	GENERATED_BODY()
	protected:


		FVector LastHit;

		void LButtonPressed();
		void LButtonReleased();
		void RButtonPressed();

		void CommandAttackMoveTo(); // Hotkey, that initiates the command. Actual command is confirmed in LButtonReleased
		void CommandStop(); // Hotkey, instant results
		void CommandHoldPosition(); // Hotkey, instant results
		void CommandDisable(); // Hotkey, instant results
		
		void SpawnAI(int _Team);
		void SpawnAI_0();
		void SpawnAI_1();
		
		void MWheelUp();
		void MWheelDowm();
		
		virtual void BeginPlay() override;
		virtual void SetupInputComponent() override;
	public:
		float ZoomSpeed; // Set in constructor
		ACustomHUD *HUD;
		TArray<ACombatTestCharacter *> SelectedActors;

		ACustomPlayerController();
};
