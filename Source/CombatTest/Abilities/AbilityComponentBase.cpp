// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/AbilityComponentBase.h"
#include "CombatTestCharacter.h"
#include "Abilities/UnitDataComponentBase.h"

UAbilityComponentBase::UAbilityComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}
//-------------------------------------------------------------------------------------------------

void UAbilityComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!UnitOwner->UnitDataComponent->IsDead())
	{
		if (bInProgress) // FIX: ??? Instead of this, stop ticking ability components on Kill()
		{
			TimeCounter += DeltaTime * TimeScale; // FIX: if attack speed is super fast, and several attacks can happen on the same tick, it can be an issue. Probably, cap attack speed, so it is never too fast.
			if (TimeCounter >= TotalTime)
			{
				UnitOwner->UnitDataComponent->bBusy = false;
				bInProgress = false;
				TimeCounter -= TotalTime; // Save overflow from previous attack, if there were no gap in between, so it doesn't truncate fractions of the tick
			}
			if (bPredelayInProgress && TimeCounter >= Predelay) // Attack commences
			{
				//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, "Foreswing");
				bPredelayInProgress = false;
				ActivateAbility();
			}
		}
		else
		{
			TimeCounter = 0; // Reset the counter if attack finished on the previous tick, but no new one was started, so it doesn't carry over to non-continuous attacks or other abilities.
		}
	}
}
//-------------------------------------------------------------------------------------------------

void UAbilityComponentBase::BeginPlay()
{
	Super::BeginPlay();
}
//-------------------------------------------------------------------------------------------------

void UAbilityComponentBase::StartActivating(ABaseCharacterClass* _Target)
{
	// Do not test for valid target - it should be tested in the calling code

	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, "ActivateAbility");
	if (bInProgress) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "ActivateAbility bInProgress"); // This should never happen

	// No need to reset TimeCounter - it is prepared in the Tick()

	UnitOwner->UnitDataComponent->bBusy = true; // Set the flag to prevent other actions from starting
	bInProgress = true; // This ability is in progress, so can't be activated again
	bPredelayInProgress = true; // Start in predelay phase
	Target = _Target;
	// += is used to factor in fractions of ticks. If attacks are continuous, they will not lose time because of truncation. If there is a gap, negative value will be zeroed out in Tick()
}
//-------------------------------------------------------------------------------------------------

bool UAbilityComponentBase::IsTargetInRange(ABaseCharacterClass* _Target)
{
	// ??? Do not test for valid target - some abilities can be activated without a target. Or use a tag for this type of abilities and test the target for other types?
	if (UnitOwner->UnitDataComponent->bBusy) return false; // Requires no action is in progress

	if (_Target == NULL) return false; // Do not allow free-form attacks for now

	// Check range
	// TODO: factor in target's size

	FVector v = _Target->GetActorLocation() - UnitOwner->GetActorLocation();
	float Dist = v.Size();

	if (Dist <= Range) // Target is in range
	{
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------------------------------

bool UAbilityComponentBase::IsTargetInAngle(ABaseCharacterClass* _Target)
{
	if (UnitOwner->UnitDataComponent->bBusy) return false; // Requires no action is in progress

	if (_Target == NULL) return false; // Do not allow free-form attacks for now

	// Check orientation
	// TODO: factor in target's size
	FVector v = _Target->GetActorLocation() - UnitOwner->GetActorLocation();
	float Dist = v.Size();

	if (FVector::DotProduct(UnitOwner->GetActorForwardVector(), v) / Dist > AngleThreshold_Cos) // Target is in front
	{
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------------------------------

bool UAbilityComponentBase::CanActivateNow(ABaseCharacterClass* _Target)
{
	// Requires no action to be in progress
	// Other abilities can use special conditions
	return UnitOwner->UnitDataComponent->bBusy;
}