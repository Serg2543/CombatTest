// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/KismetMathLibrary.h"

#include "CombatTestCharacter.h"
#include "Abilities/AbilityComponentBase.h"
#include "Abilities/UnitDataComponentBase.h"

void ACustomAIController::Tick_HoldPosition(float DeltaSeconds)
{
	// ??? The same, as Idle, but it will be checked later to prevent movement
	AcquireTarget();
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::Tick_Idle(float DeltaSeconds)
{
	AcquireTarget();
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::Tick_Move(float DeltaSeconds)
{
	MoveToLocation(MoveDestination);
	// GetCharacterMovement()->IsMovementInProgress() is always false
	if (GetMoveStatus() == EPathFollowingStatus::Idle) // Check, if destination is reached
	{
		CommandStop();
	}
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::Tick_AttackMove(float DeltaSeconds)
{
	// FIX: not tested yet
		// ??? Do not even need to use MoveTo - it will be handled by general logic later
	AcquireTarget();
	if (Target != NULL) // If a target is found
	{
		MoveTo(Target->GetActorLocation()); // Chase the target
		// Stop after getting in range will be handled manually
	}
	else // If no target is found
	{
		MoveTo(MoveDestination); // Resume normall movement
		Tick_Move(DeltaSeconds);
	}
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::Tick_AttackTarget(float DeltaSeconds)
{
	// FIX: not tested yet
		// ??? Do not even need to use MoveTo - it will be handled by general logic later
	if (Target != NULL)
	{
		MoveTo(Target->GetActorLocation()); // Chase the target
		// Stop after getting in range will be handled manually
	}
	else
	{
		CommandStop();
		// Can store target's last location and go there
	}
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (Target && (Target->UnitDataComponent->IsDead() || Target->IsPendingKill())) // Check, if the target is dead in the game or destroyed in the engine, and invalidate it explicitly, so it is easier to handle
	{
		// (...) || (Ability && !Ability->IsTargetValid(Target)) // When abilities are implemented fully
		Target = NULL;
	}
	// -------
	if (UnitCommand == EUC_Disable)
	{
		return; // Return explicitly, just in case
	}
	else if (Unit->UnitDataComponent->bBusy)
	{
		// An ability is in progress, so most behavior is not available			
		// But there can be actions, that can still be performed			
		if (Target)
		{
			RotateToTarget(DeltaSeconds); // Can still "aim" the attack or ability to keep the target in angle			
		}
	}
	else // If !bBusy
	{
		// Resolve commands			
		if (UnitCommand == EUC_Idle)
		{
			Tick_Idle(DeltaSeconds);
			// Can roam around, etc		
		}
		else if (UnitCommand == EUC_Move)
		{
			Tick_Move(DeltaSeconds);
		}
		else if (UnitCommand == EUC_AttackMove)
		{
			Tick_AttackMove(DeltaSeconds);
		}
		else if (UnitCommand == EUC_AttackTarget)
		{
			Tick_AttackTarget(DeltaSeconds);
		}
		else if (UnitCommand == EUC_HoldPosition)
		{
			Tick_HoldPosition(DeltaSeconds);
		}
		// -------
		if (Target)
		{
			// If any command found a valid target, execute the same logic (almost)			
			// Get in range, rotate towards it and activate the ability			
			if (!Ability->IsTargetInRange(Target))
			{
				if (UnitCommand != EUC_HoldPosition)
				{
					MoveTo(Target->GetActorLocation());
				}
				else
				{
					RotateToTarget(DeltaSeconds); // Keep rotating to face the target, when it gets in range
				}
			}
			else
			{
				StopMovement(); // Prevent AI derping around
				if (!Ability->IsTargetInAngle(Target))
				{
					RotateToTarget(DeltaSeconds);
				}
				else
				{
					Ability->StartActivating(Target);
				}
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::RotateToTarget(float DeltaSeconds)
{
	FRotator rot = UKismetMathLibrary::FindLookAtRotation(Unit->GetActorForwardVector(), Target->GetActorLocation() - Unit->GetActorLocation());
	// For some reason, regular acos sometimes returns values < -1000 (noticed in AbilityComponentBase->IsInAngle, but there it seems to work)
	//float a = acos(FVector::DotProduct(Unit->GetActorForwardVector(), (Target->GetActorLocation() - Unit->GetActorLocation()).GetSafeNormal()));
	float a = UKismetMathLibrary::Acos(FVector::DotProduct(Unit->GetActorForwardVector(), (Target->GetActorLocation() - Unit->GetActorLocation()).GetSafeNormal()));
	
	a += 0.01; // Prevent /0
	float TurnSpeed = 0.5;
	rot = FMath::RInterpTo(Unit->GetActorRotation(), rot, DeltaSeconds, TurnSpeed / (a / (2 * 3.14159265))); // It is scaled by angle, so un-scale it, so it becomes linear again
	Unit->SetActorRotation(rot);
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
	// Can treat current target differently to prevent switching targets back and forth
	ACombatTestCharacter *NewTarget = NULL;
	float DistMinSquared = AcquisitionRange * AcquisitionRange;
	float DistCurSquared;
	FVector pos = Unit->GetActorLocation();
	FVector dest;

	for (int i = 0; i < OutActors.Num(); i++)
	{
		if (!((ACombatTestCharacter *)OutActors[i])->UnitDataComponent->IsDead() && ((ACombatTestCharacter *)OutActors[i])->Team != Unit->Team)
		{
			dest = ((ACombatTestCharacter *)OutActors[i])->GetActorLocation();
			if ((DistCurSquared = (dest - pos).SizeSquared()) < DistMinSquared)
			{
				NewTarget = (ACombatTestCharacter *)OutActors[i];
				DistMinSquared = DistCurSquared;
			}
		}
	}
	Target = NewTarget;
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::CommandHoldPosition() // Can have parameters for aggro behavior
{
	Target = NULL;
	CommandStop();
	UnitCommand = EUC_HoldPosition;
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::CommandStop()
{
	Target = NULL;
	StopMovement(); // If it is already not moving, it seems to not matter
	UnitCommand = EUC_Idle; // The unit is idle now
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::CommandDisable()
{
	Target = NULL;
	CommandStop(); // If it is already not moving, it seems to not matter
	UnitCommand = EUC_Disable; // The unit is idle now
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::CommandMoveTo(FVector _MoveDestination)
{
	Target = NULL;
	MoveDestination = _MoveDestination;
	//MoveToLocation(_MoveDestination, UPathFollowingComponent::DefaultAcceptanceRadius, true, true, false, false, NULL, true);
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

void ACustomAIController::CommandAttackTarget(ACombatTestCharacter *_Target)
{
	Target = _Target;
	UnitCommand = EUC_AttackTarget;
}
//-------------------------------------------------------------------------------------------------

void ACustomAIController::OnPossess(APawn* InPawn)
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

ACustomAIController::ACustomAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
}