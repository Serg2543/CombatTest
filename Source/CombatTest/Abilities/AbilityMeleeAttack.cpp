// Fill out your copyright notice in the Description page of Project Settings.

#include "Abilities/AbilityMeleeAttack.h"
#include "CombatTestCharacter.h"

/*
#include "AbilityMeleeAttack.h"
#include "../CombatTestCharacter.h"
*/

void UAbilityMeleeAttack::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	AttackDelayCounter += DeltaTime;
	if (AttackDelayCounter > AttackDelay)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, "Attack"); // It ticks even without registering or attaching to anything
		AttackDelayCounter -= AttackDelay;
	}
}
//-------------------------------------------------------------------------------------------------

void UAbilityMeleeAttack::ActivateAbility(ACombatTestCharacter *_Target)
{
	int coin = FMath::RandRange(0, 1);
	ACombatTestCharacter *UnitToKill;
	if (coin) UnitToKill = _Target;
	else UnitToKill = UnitOwner;
	UnitToKill->Kill();
}
//-------------------------------------------------------------------------------------------------

bool UAbilityMeleeAttack::CanActivateNow(ACombatTestCharacter*_Target)
{
	if ((_Target->GetActorLocation() - UnitOwner->GetActorLocation()).SizeSquared() < Range * Range)
	{
		return true;
	}
	return false;
}
//-------------------------------------------------------------------------------------------------

UAbilityMeleeAttack::UAbilityMeleeAttack()
{

}
//-------------------------------------------------------------------------------------------------

UAbilityMeleeAttack::~UAbilityMeleeAttack()
{

}
//-------------------------------------------------------------------------------------------------