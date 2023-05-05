// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/AbilityComponentBase.h"

#include "BaseCharacterClass.h" // Replace the character class name there

#include "AbilityMeleeAttack.generated.h"

UCLASS()
class COMBATTEST_API UAbilityMeleeAttack : public UAbilityComponentBase
{
	GENERATED_BODY()
	protected:
		virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		virtual void ActivateAbility() override;
	public:
		virtual void StartActivating(ABaseCharacterClass * _Target) override;
		virtual bool CanActivateNow(ABaseCharacterClass *_Target) override;

		UAbilityMeleeAttack();
		~UAbilityMeleeAttack();
};
