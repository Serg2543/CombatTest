// Fill out your copyright notice in the Description page of Project Settings.

/*
#include "Abilities/UnitDataComponentBase.h"
#include "CombatTestCharacter.h"
#include "Abilities/AbilityMeleeAttack.h"
*/


#include "UnitDataComponentBase.h"
#include "../CombatTestCharacter.h"
#include "AbilityMeleeAttack.h"


void UUnitDataComponentBase::SetOwner(ACombatTestCharacter *_UnitOwner)
{
	UnitOwner = _UnitOwner;
	for (int i = 0; i < Abilities.Num(); i++)
		Abilities[i]->UnitOwner = UnitOwner;
}
//-------------------------------------------------------------------------------------------------

// Called when the game starts
void UUnitDataComponentBase::BeginPlay()
{
	Super::BeginPlay();
	// ...	
}
//-------------------------------------------------------------------------------------------------

// Called every frame
void UUnitDataComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
//-------------------------------------------------------------------------------------------------

// Sets default values for this component's properties
UUnitDataComponentBase::UUnitDataComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	UAbilityMeleeAttack* ma = CreateDefaultSubobject<UAbilityMeleeAttack>(TEXT("MeleeAttack")); // Have no idea, what exactly it does - copying the approach with the camera from dafault code above
	//ma->AddInstanceComponent(UnitDataComponent);
	//ma->RegisterComponent(); // Exception
	//ma->UnitOwner = UnitOwner;
	Abilities.Add(ma); // ??? How to add it to a default array of components? Or simply manually destroy it?

	//UAbilityMeleeAttack *ma = new UAbilityMeleeAttack(_UnitOwner);
}
//-------------------------------------------------------------------------------------------------