// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/AbilityMeleeAttack.h"
#include "CombatTestCharacter.h"
#include "Abilities/UnitDataComponentBase.h"

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
	return Super::CanActivateNow(_Target) &&
		_Target != NULL; // Do not allow free-form attacks for now
}
//-------------------------------------------------------------------------------------------------

UAbilityMeleeAttack::UAbilityMeleeAttack()
{
	
	Range = 100; // Default for test melee range
	BaseDamage = 20;
	AngleThreshold = 20 * 3.14159265 / 180;
}
//-------------------------------------------------------------------------------------------------

UAbilityMeleeAttack::~UAbilityMeleeAttack()
{

}
//-------------------------------------------------------------------------------------------------