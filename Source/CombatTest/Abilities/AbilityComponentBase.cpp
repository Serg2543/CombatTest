// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/AbilityComponentBase.h"
#include "CombatTestCharacter.h"

/*
#include "AbilityComponentBase.h"
#include "AbilityMeleeAttack.h"
*/

// Sets default values for this component's properties
UAbilityComponentBase::UAbilityComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	//UnitOwner = _UnitOwner;
	
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "UAbilityComponentBase constructor");
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
			TimeCounter = 0; // Reset the counter if an attack finished on the previous tick, but no new one was started, so it doesn't carry over to non-continuous attacks.
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
	// Do not test for valid target - see CanActivateNow()

	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, "ActivateAbility");
	// CanActivateNow should be already called in the higher level code, so no need to test it again
	if (bInProgress) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "ActivateAbility bInProgress"); // This should never happen

	// No need to reset TimeCounter - it is prepared in the Tick()

	UnitOwner->UnitDataComponent->bBusy = true; // Set the flag to prevent other actions from starting
	bInProgress = true;
	bPredelayInProgress = true;
	Target = _Target;
	// += is used to factor in fractions of ticks. If attacks are continuous, they will not lose time because of truncation. If there is a gap, negative value will be zeroed out in Tick()
}
//-------------------------------------------------------------------------------------------------