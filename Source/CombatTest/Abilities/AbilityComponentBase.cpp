// Fill out your copyright notice in the Description page of Project Settings.

/*
#include "Abilities/AbilityComponentBase.h"
#include "Abilities/AbilityMeleeAttack.h"
*/

#include "AbilityComponentBase.h"
#include "AbilityMeleeAttack.h"

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

// Called when the game starts
void UAbilityComponentBase::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}
//-------------------------------------------------------------------------------------------------

// Called every frame
void UAbilityComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
//-------------------------------------------------------------------------------------------------
