// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CustomAIController.generated.h"

class ACombatTestCharacter;
class UAbilityComponentBase;

enum EUnitCommand {
	EUC_Idle, // No corders
	EUC_HoldPosition, // Don't move, but attempt to acquire a target and attack, if the target is in range.
	EUC_Disable, // Debug mode: stay in place, do not take any actions
	EUC_Move, // Move order. Move to the destination point, ignore enemies.
	EUC_AttackMove, // A-move order. Move to the destination point, but if an enemy is within acquisition range, attack the enemy.
	EUC_AttackTarget // Direct attack order (focus fire). Move in range of the target, ignore other enemies.	
	};

UCLASS()
class COMBATTEST_API ACustomAIController : public AAIController
{
	GENERATED_BODY()
	protected:
		void Tick_HoldPosition(float DeltaSeconds);
		void Tick_Idle(float DeltaSeconds);
		void Tick_Move(float DeltaSeconds);
		void Tick_AttackMove(float DeltaSeconds);
		void Tick_AttackTarget(float DeltaSeconds);
		
		virtual void Tick(float DeltaSeconds) override;
		
		void RotateToTarget(float DeltaSeconds); // Manually update rotation
			float TestT = 0;

		float AcquisitionRange = 10000; // Set for the whole world for now
		void AcquireTarget();

		virtual void OnPossess(APawn *InPawn) override;
		void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	public:
		EUnitCommand UnitCommand = EUC_Idle;
		ACombatTestCharacter *Unit = NULL; // Possessed unit
		ACombatTestCharacter *Target = NULL; // Current target
		UAbilityComponentBase *Ability = NULL; // Current ability planned to be used on the target

		FVector MoveDestination = FVector(0, 0, 0);

		void CommandHoldPosition(); // Can have parameters for aggro behavior
		void CommandStop();
		void CommandDisable();
		void CommandMoveTo(FVector _MoveDestination);
		void CommandAttackMoveTo(FVector _MoveDestination);
		void CommandAttackTarget(ACombatTestCharacter *_Target);

		ACustomAIController(const FObjectInitializer& ObjectInitializer);
};
