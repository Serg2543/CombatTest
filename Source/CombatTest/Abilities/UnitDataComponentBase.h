// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "BaseCharacterClass.h" // Replace the character class name there

#include "UnitDataComponentBase.generated.h"

class UAbilityComponentBase;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMBATTEST_API UUnitDataComponentBase : public UActorComponent
{
	GENERATED_BODY()
protected:
	ABaseCharacterClass *UnitOwner;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;

	bool bIsDead = false;
public:	
	UPROPERTY(EditDefaultsOnly, Category = "UnitData")
		float HPMax = 100;
	float HP = HPMax;
	UPROPERTY(EditDefaultsOnly, Category = "UnitData")
		float Damage;

	bool bBusy = false; // Flag to determine if any action is in progress;

	TArray<UAbilityComponentBase *> Abilities; // Use a custom array of components for more straightforward access, but add to the main array for memory management

	virtual void TakeDamage(float DamageTaken);
	void Kill();
		bool IsDead() {	return bIsDead; }
		float CorpseTimer = 3; // Time before the corpse is despawned (temporary approach)

	void SetOwner(ABaseCharacterClass *_UnitOwner); // Set owner for this component, all abilities and items
	ABaseCharacterClass *GetOwner() { return UnitOwner; }

	/*
		// ???
		class UStatusEffects *StatusEffects; // Status effects can be a separate class, that holds a static set of values for each type of effect, rather than making each instance a separate object.
	*/
	UUnitDataComponentBase();
};
