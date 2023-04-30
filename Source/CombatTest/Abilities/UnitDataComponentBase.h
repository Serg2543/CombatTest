// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

/*
#include "Abilities/AbilityComponentBase.h"
*/

#include "AbilityComponentBase.h"
//#include "CombatTestCharacter.h"

#include "UnitDataComponentBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMBATTEST_API UUnitDataComponentBase : public UActorComponent
{
	GENERATED_BODY()
protected:
	class ACombatTestCharacter* UnitOwner;

	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditDefaultsOnly, Category = "UnitData")
		float HPMax;
	float HP;
	UPROPERTY(EditDefaultsOnly, Category = "UnitData")
		float Damage;

	TArray<UAbilityComponentBase *> Abilities; // Use a custom array of components for more straightforward access, but add to the main array for memory management

	void SetOwner(class ACombatTestCharacter* _UnitOwner); // Set owner for all abilities and items
	class ACombatTestCharacter *GetOwner() { return UnitOwner; }
	/*
		class UStatusEffects *StatusEffects; // Status effects can be a separate class, that holds a static set of values for each type of effect, rather than making each instance a separate object.
	*/

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Sets default values for this component's properties
	UUnitDataComponentBase();
};
