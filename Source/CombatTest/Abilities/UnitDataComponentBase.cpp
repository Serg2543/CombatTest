// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/UnitDataComponentBase.h"
//#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
//#include "Components/SlateWrapperTypes.h"

#include "Abilities/AbilityMeleeAttack.h"
#include "CombatTestCharacter.h"
#include "CustomAIController.h"
#include "CustomHUD.h"
#include "CustomPlayerController.h"
#include "CustomGameInstance.h"

void UUnitDataComponentBase::TakeDamage(float DamageTaken)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, "Take damage");
	HP -= DamageTaken;
	if (HP <= 0)
	{
		Kill();
	}
}
//-------------------------------------------------------------------------------------------------

void UUnitDataComponentBase::Kill()
{
	// FIX: ??? Notify the unit

	// Remove the actor from selection lists
	ACustomPlayerController *CustomPlayerController = (ACustomPlayerController *)UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ACustomHUD* CustomHUD = (ACustomHUD *)CustomPlayerController->GetHUD();
	UnitOwner->SetSelected(false);
	CustomPlayerController->SelectedActors.Remove(UnitOwner);
	CustomHUD->SelectedActorsDragged.Remove(UnitOwner);

	// Disable AI
	UnitOwner->CustomAIController->StopMovement();
	UnitOwner->CustomAIController->UnPossess();
	UnitOwner->CustomAIController->Destroy(); // Destroy the controller explicitly, because it it is not automatically destroyed after Unpossess()
	bIsDead = true;

	// Turn off collision with units
	UnitOwner->SetActorEnableCollision(false);
	// Change visual appearance
	UnitOwner->CompBody->SetMaterial(0, ((UCustomGameInstance *)GetWorld()->GetGameInstance())->MaterialDead);
	// Use a proper death animation
}
//-------------------------------------------------------------------------------------------------

void UUnitDataComponentBase::SetOwner(ABaseCharacterClass *_UnitOwner)
{
	UnitOwner = _UnitOwner;
	for (int i = 0; i < Abilities.Num(); i++)
		Abilities[i]->UnitOwner = UnitOwner;
}
//-------------------------------------------------------------------------------------------------

void UUnitDataComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsDead())
	{
		CorpseTimer -= DeltaTime;
		if (CorpseTimer <= 0) UnitOwner->Destroy(); // Can use another phase: fade out, fall through the floor, etc
	}
}
//-------------------------------------------------------------------------------------------------

void UUnitDataComponentBase::BeginPlay()
{
	Super::BeginPlay();
}
//-------------------------------------------------------------------------------------------------

UUnitDataComponentBase::UUnitDataComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	UAbilityMeleeAttack *AbilityMeleeAttack= CreateDefaultSubobject<UAbilityMeleeAttack>(TEXT("MeleeAttack")); // Have no idea, what exactly it does - copying the approach with the camera from dafault code above
	//AddInstanceComponent(UnitDataComponent); // Worked without that, but seems like it is a proper thing to do (or something like this to let the engine to handle the component). Should the actor be the object, that calls this?
	Abilities.Add(AbilityMeleeAttack); // ??? How to add it to a default array of components? Or simply manually destroy it?


	CorpseTimer = 0.3;
}
//-------------------------------------------------------------------------------------------------