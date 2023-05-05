// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CombatTestCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UDecalComponent;
class UStaticMeshComponent;
class UWidgetComponent;
class UUserWidget;
class UProgressBar;

class ACustomAIController;
class UUnitDataComponentBase;

UCLASS(Blueprintable)
class ACombatTestCharacter : public ACharacter
{
	GENERATED_BODY()
protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

public:
	int Team = -1;
	UMeshComponent *CompBody = NULL;
	UStaticMeshComponent *CompHead = NULL;
	UStaticMeshComponent *CompFoot_L = NULL;
	UStaticMeshComponent *CompFoot_R = NULL;
		FVector Foot_L_BaseLoc;
		FVector Foot_R_BaseLoc;
		float MoveAnimPeriod = 1;
		float MoveAnimTime = 0;
	UStaticMeshComponent *CompHand_R = NULL;
	UStaticMeshComponent *CompHand_L = NULL;
	//USkeletalMeshComponent* MeshBody = NULL;
	//UMaterialInstanceDynamic* MaterialBodyTeam0 = NULL, * MaterialBodyTeam1 = NULL, * MaterialBodyDead = NULL;
	UWidgetComponent *WidgetComponent;
	UUserWidget *UserWidget;
	UProgressBar *WidgetHPBar = NULL;
	UStaticMeshComponent *SelectionCircle = NULL;
	
	ACustomAIController* CustomAIController; // Store it, so it doesn't have to be found every tick. Initialize in BeginPlay
	UUnitDataComponentBase *UnitDataComponent = NULL;

	void SetSelected(bool _Selected);
	ACombatTestCharacter();
};