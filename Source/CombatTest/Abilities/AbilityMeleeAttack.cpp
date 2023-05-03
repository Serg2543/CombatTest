// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/AbilityMeleeAttack.h"
#include "CombatTestCharacter.h"
#include "Abilities/UnitDataComponentBase.h"

/*
#include "AbilityMeleeAttack.h"
#include "../CombatTestCharacter.h"
*/

void UAbilityMeleeAttack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
//-------------------------------------------------------------------------------------------------

void UAbilityMeleeAttack::ActivateAbility()
{
	if (Target != NULL && !Target->UnitDataComponent->IsDead() && !Target->IsPendingKill())
	{
		Target->UnitDataComponent->TakeDamage(BaseDamage);
	}
	// If Target is invalid, simply finish the sequence
}

void UAbilityMeleeAttack::StartActivating(ABaseCharacterClass *_Target)
{
	Super::StartActivating(_Target);
}
//-------------------------------------------------------------------------------------------------

bool UAbilityMeleeAttack::CanActivateNow(ABaseCharacterClass *_Target)
{
	// Do not test for valid target - some abilities can be activated without a target. Or use a tag for this type of abilities and test the target for other types?

	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, "CanActivateNow");
	if (!UnitOwner->UnitDataComponent->bBusy && // No action is in progress
		//!bInProgress && // Current action is not in progress - seems unnecessary, because it will also set bBusy
		(_Target->GetActorLocation() - UnitOwner->GetActorLocation()).SizeSquared() < Range * Range) // Target is in range
	{
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------------------------------

UAbilityMeleeAttack::UAbilityMeleeAttack()
{
	Range = 200; // Default for test melee range
	BaseDamage = 20;
}
//-------------------------------------------------------------------------------------------------

UAbilityMeleeAttack::~UAbilityMeleeAttack()
{

}
//-------------------------------------------------------------------------------------------------