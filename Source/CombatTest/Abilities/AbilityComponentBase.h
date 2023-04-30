// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilityComponentBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMBATTEST_API UAbilityComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	
	UAbilityComponentBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	class ACombatTestCharacter *UnitOwner = NULL; // Use owner of the component?
	class ACombatTestCharacter *Target = NULL; // Target, that the ability was used on
	float EnergyCost = 0;
	float Range = 200; // Default for test melee range

	// Some information about valid targets (enemy, ally, self, ground, etc)
	// bool IsAttack = false; // Ability can used as an autoattack against previous target
	/*
	virtual float Estimate(class ABaseCharacter* Target) {return 0;} // AI can estimate the value of abilities and pick the best one
	virtual bool IsTargetValid(class ABaseCharacter* Target) {return true;}
	*/
	virtual void ActivateAbility(class ACombatTestCharacter *_Target) {}
	virtual bool CanActivateNow(class ACombatTestCharacter *_Target) { return false; } // To determine, if AI has to move closer or do something else to be able to use the ability. Can return Enum to specify the reason of fail.
	/*
	virtual bool CanActivate(); // Can be used to gray out abilities in the UI in GAS it is "CanActivateAbility"

	Enum ProjectileType; // Instant, melee, simple, ... May be unnecessary, because each ability will create an object of required type and behavior will be defined in the code
	void CreateProjectile(class AProjectile *Projectile); // Use(...) should create projectile object and set it's parameters - each ability can set it's own special projectile type and parameters
	// ??? Projectile can be stored as SubclassOf properties, so they can be set in BPs

	*/
};
