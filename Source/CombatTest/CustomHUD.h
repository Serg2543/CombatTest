// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CombatTestCharacter.h"
#include "CustomHUD.generated.h"

/**
 * 
 */
UCLASS()
class COMBATTEST_API ACustomHUD : public AHUD
{
	GENERATED_BODY()
	protected:
		
	public:
		FVector2D InitialPoint;
		FVector2D CurrentPoint;
		
		virtual void DrawHUD() override;
		FVector2D GetMousePos2D();

		bool bStartSelecting = false;
		TArray<ACombatTestCharacter*> SelectedActorsDragged;
		
};
