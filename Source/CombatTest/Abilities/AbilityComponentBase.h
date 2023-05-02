// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Abilities/UnitDataComponentBase.h"

#include "AbilityComponentBase.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMBATTEST_API UAbilityComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	
	UAbilityComponentBase();

protected:
	virtual void ActivateAbility() {} //  // Actually apply the ability, manifest effects, spawn projectiles, etc. ??? Call it "Payload"?

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	ABaseCharacterClass *UnitOwner = NULL; // Use owner of the component?
	ABaseCharacterClass *Target = NULL; // Target, that the ability was used on

	bool bInProgress = false; // Flag to determine if the ability is in progress

	/*
	FIX: For now using these for a simple attack, but they should be in a different place.
		??? Use AbilityAttack (AbilityRepeatable ?) for attacks with autoattack and/or autocast possibility
	??? Can be a general CD? But abilities can have attack delay or cast delay and CD at the same time
	It should be a global ability use block - it attack is in progress, casting is not possible, etc
		May be movement too (even canceling backswing can be not possible)
*/

	
	float TotalTime = 1.5; // Total time to finish ability activation sequence and become available for another action
	float Predelay = 0.5; // Foreswing, cast point, windup, etc - time from starting the animation to actually activating the ability
	float TimeScale = 1; // Attack speed, etc
	float TimeCounter = 0; // Current tick counter

	float BaseDamage = 20;
	float Range = 200; // Default for test melee range
	bool bPredelayInProgress = false; // Flag to determine, if attack has not connected yet, similar to bInProgress

	float EnergyCost = 0;

	// Some information about valid targets (enemy, ally, self, ground, etc)
	// bool IsAttack = false; // Ability can used as an autoattack against previous target
	/*
	virtual float Estimate(class ABaseCharacter* Target) {return 0;} // AI can estimate the value of abilities and pick the best one
	virtual bool IsTargetValid(class ABaseCharacter* Target) {return true;}
	*/
	virtual void StartActivating(ABaseCharacterClass* _Target); // Start activation sequence: predelay, payload, recovery (for an attack: foreswing, resolve the hit, backswing)
		/* TODO: For player hero abilities, add a function to initiate targeting, etc for some abilities. ??? Should it be a subclass or a flag?
				This function will call StartActivating() after confirmation and all checks
				But AI will not need targeting, so StartActivating() should be accessible right away
		*/
	virtual bool CanActivateNow(ABaseCharacterClass *_Target) { return false; } // To determine, if AI has to move closer or do something else to be able to use the ability. Can return Enum to specify the reason of fail.
		// ??? Add another function, that checks special conditions only, but not if any action is in progress - it will allow to finish current action and use the ability
			// !!!??? If any action is in progress, other abilities should be not attempted at all (the whole loop throug potential targets should be bypassed)
				// The whole AI decision making can be bypassed, unless it can be done in parallel with current action
		// ??? For free-form player abilities can return true by default (but check bBusy, requirements, etc)

	/*
	virtual bool CanActivate(); // Can be used to gray out abilities in the UI in GAS it is "CanActivateAbility"

	Enum ProjectileType; // Instant, melee, simple, ... May be unnecessary, because each ability will create an object of required type and behavior will be defined in the code
	void CreateProjectile(class AProjectile *Projectile); // Use(...) should create projectile object and set it's parameters - each ability can set it's own special projectile type and parameters
	// ??? Projectile can be stored as SubclassOf properties, so they can be set in BPs

	*/
};
