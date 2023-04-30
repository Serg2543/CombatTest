// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Abilities/AbilityComponentBase.h"
//#include "AbilityComponentBase.h"

#include "AbilityMeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class COMBATTEST_API UAbilityMeleeAttack : public UAbilityComponentBase
{
	GENERATED_BODY()
	protected:
		virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	public:

		float AttackDelay = 1.7;
		float AttackDelayCounter = 0;
		virtual void ActivateAbility(class ACombatTestCharacter* _Target) override;
		virtual bool CanActivateNow(class ACombatTestCharacter*_Target) override;

		UAbilityMeleeAttack();
		~UAbilityMeleeAttack();
};
