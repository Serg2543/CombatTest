// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/CrowdFollowingComponent.h"

/*
#include "AbilityComponentBase.h"
*/

#include "Abilities/AbilityComponentBase.h"


void ACustomAIController::Tick_Idle(float DeltaSeconds)
{
	if (Target == NULL) AcquireTarget(); // Attempt to acquire a target
	if (Target == NULL) // If there is still no target
	{
		StopMovement();
	}
	else
	{
		UnitCommand = EUC_AttackMove; // A-move to current position. 
		// FIX: use MoveDestination to store current position, so the unit will chase the target, then return.
	}
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::Tick_Move(float DeltaSeconds)
{
	// GetCharacterMovement()->IsMovementInProgress() is always false
	if (GetMoveStatus() == EPathFollowingStatus::Idle) // Check, if destination is reached
	{
		CommandStop();
	}
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::Tick_AttackMove(float DeltaSeconds)
{
	if (Target == NULL) AcquireTarget(); // Attempt to acquire a target
	if (Target == NULL) // If there is still no target
	{
		Tick_Move(DeltaSeconds); // Keep moving normally
	}
	else
	{
		// Instead of resolving it here, add an attack order and resolve there (use a stack of orders, so if the target is dead, the unit will return to previous order - a-move)
		FVector MoveLocation = Target->GetActorLocation();
		
		// FIX: use MoveDestination to store destination, so the unit will chase the target, then resume a-move.
		/*
		// Initial melee attack resolution: kill a random unit instantly
		float MeleeRange = 200; // Temp
		if ((MoveLocation - Unit->GetActorLocation()).SizeSquared() < MeleeRange * MeleeRange) // If actors get close, destroy one of the actors
		{
			//
			int coin = FMath::RandRange(0, 1);
			ACombatTestCharacter* UnitToKill;
			if (coin) UnitToKill = Target;
			else UnitToKill = Unit;
			UnitToKill->Kill();
		}
		else
		{
			MoveToLocation(MoveLocation);
		}
		*/
		// Abilities added
		if (Ability->CanActivateNow(Target)) // If can use the planned ability right now, do it
		{
			Ability->ActivateAbility(Target);
		}
		else
		{
			MoveToLocation(MoveLocation);
			// It calls OnMoveCompleted each time. In this case, it doesn't matter, but may be a problem at some point.
			// Is it possible to determine the reason of MoveCompleted being called? It has some parameters. Getting to the closest point, if destination is unreachable, or timeout can be important. In other cases, manual check can be good enough.
			// Do not use MoveToActor - sometimes destination will require updates every tick anyway (the unit will want to move not directly towards the target: anticipate and intercept, kite, etc). But in that case, reaching destination will be irrelevant - it will be a custom check anyway?
		}
		/*
			// Proper behavior
			// The unit will have weapons, stats, etc, and execute the attack

		if ((MoveLocation - Unit->GetActorLocation()).SizeSquared() < Unit->AttackRange * Unit->AttackRange)
		{
			Unit->Attack(Target);
			//
			int coin = FMath::RandRange(0, 1);
			ACombatTestCharacter* UnitToKill;
			if (coin) UnitToKill = Target;
			else UnitToKill = Unit;
			UnitToKill->Kill();
		}
		else
		{
			CustomAIController->MoveToLocation(MoveLocation);
		}
		*/
	}
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::Tick_AttackTarget(float DeltaSeconds)
{
	Tick_AttackMove(DeltaSeconds); // For now, there is no difference - if the unit has a target, it will move towards it
	// Can have different logic, if target is destroyed: do not continue moving to a-move destination, but stop in place
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::Tick_HoldPosition(float DeltaSeconds)
{
	// Do not use for now
	/*
		// ??? The same, as AttackMove, but add checks to move only if the command is actually a-move
		Tick_AttackMove(DeltaSeconds);
		// Can add a check for moving too far away
	*/
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	/*
	Unit = Cast<ACombatTestCharacter>(GetPawn());
	if (Unit == NULL) return;
	*/

	if (Target && (Target->IsDead || Target->IsPendingKill())) Target = NULL; // Check, if the target is dead in the game or destroyed in the engine
	

	if (!Unit->IsDead) // It seems redundant - the unit will be unpossessed
		// But have to check for other unit states, that prevent acting (attacking, hit recovery, stun, etc) - they will be handled by the unit. It can be a general "BlocksAI" flag? Or set CanTick directly?
		// Some actions can be interrupted intentionally: can make sense to start a dodge roll during an attack		
	{
		if (UnitCommand == EUC_Idle) // The unit is idle
		{
			Tick_Idle(DeltaSeconds);
		}
		else if (UnitCommand == EUC_Move) // A manual move order was issued
		{
			Tick_Move(DeltaSeconds);
		}
		else if (UnitCommand == EUC_AttackMove) // A-move
		{
			Tick_AttackMove(DeltaSeconds);
		}	
	}
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::AcquireTarget()
{
	// Bruteforce approach
	TArray<AActor*> OutActors;
	TArray< AActor* > ActorsToIgnore;
	ActorsToIgnore.Add(Unit);

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACombatTestCharacter::StaticClass(), OutActors);

	if (OutActors.Num() == 0) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Find actors - no results");
	Target = NULL;
	float DistMinSquared = AcquisitionRange * AcquisitionRange;
	float DistCurSquared;
	FVector pos = Unit->GetActorLocation();
	FVector dest;

	for (int i = 0; i < OutActors.Num(); i++)
	{
		if (!((ACombatTestCharacter*)OutActors[i])->IsDead && ((ACombatTestCharacter*)OutActors[i])->Team != Unit->Team)
		{
			dest = ((ACombatTestCharacter*)OutActors[i])->GetActorLocation();
			if ((DistCurSquared = (dest - pos).SizeSquared()) < DistMinSquared)
			{
				Target = (ACombatTestCharacter*)OutActors[i];
				DistMinSquared = DistCurSquared;
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::OnPossess(APawn *InPawn)
{
	Super::OnPossess(InPawn);
	Unit = Cast<ACombatTestCharacter>(InPawn);
	if (Unit == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "OnPossess Unit == NULL");
	Ability = Unit->UnitDataComponent->Abilities[0];
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Move completed");
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::CommandStop()
{
	Target = NULL;
	StopMovement(); // If it is already not moving, it seems to not matter
	UnitCommand = EUC_Idle; // The unit is idle now
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::CommandMoveTo(FVector _MoveDestination)
{
	Target = NULL;
	MoveDestination = _MoveDestination;
	MoveToLocation(_MoveDestination, UPathFollowingComponent::DefaultAcceptanceRadius, true, true, false, false, NULL, true);
	UnitCommand = EUC_Move;
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::CommandAttackMoveTo(FVector _MoveDestination)
{
	Target = NULL;
	CommandMoveTo(_MoveDestination);
	UnitCommand = EUC_AttackMove;
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::CommandAttackTarget(ACombatTestCharacter* _Target)
{
	Target = _Target;
	UnitCommand = EUC_AttackTarget;
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::CommandHoldPosition() // Can have parameters for aggro behavior
{
	Target = NULL;
	CommandAttackMoveTo(Unit->GetActorLocation()); // A-move to current location, add checks for actual command to prevent actual movement and reaching destination
	UnitCommand = EUC_HoldPosition;
}
//-------------------------------------------------------------------------------------------------

ACustomAIController::ACustomAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
}