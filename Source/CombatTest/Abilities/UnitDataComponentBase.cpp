// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/UnitDataComponentBase.h"
//#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SlateWrapperTypes.h"

#include "Abilities/AbilityMeleeAttack.h"
#include "CombatTestCharacter.h"
#include "CustomAIController.h"
#include "CustomHUD.h"
#include "CustomPlayerController.h"
#include "CustomGameInstance.h"

/*
#include "UnitDataComponentBase.h"
#include "../CombatTestCharacter.h"
#include "AbilityMeleeAttack.h"
*/

void UUnitDataComponentBase::TakeDamage(float DamageTaken)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, "Take damage");
	HP -= DamageTaken;
	if (HP <= 0)
	{
		Kill(); // FIX: ??? Notify the unit
	}
	/*
		// Moved widget update logic into the character - it checks scale to set visibility, so it has to be updated every tick. And it makes more sense architrecture-wise: the component won't always know, what components and properties the character has.
	else if (UnitOwner->WidgetHPBar)
	{
		//Percent = 0.9; // Debug
		//UnitOwner->WidgetHPBar->Percent = Percent; // Doesn't work, have to use Set...() method explicitly
		UnitOwner->WidgetHPBar->SetPercent(Percent);
		UnitOwner->WidgetHPBar->SetVisibility(Percent < 1 ? ESlateVisibility::Visible : ESlateVisibility::Hidden); // Show only if < 100%
	}
	else GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "WidgetHPBar == NULL");
	*/
}
//-------------------------------------------------------------------------------------------------

void UUnitDataComponentBase::Kill()
{
	// Remove the actor from selection lists
	ACustomPlayerController *CustomPlayerController = (ACustomPlayerController *)UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ACustomHUD* CustomHUD = (ACustomHUD *)CustomPlayerController->GetHUD();
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
	UnitOwner->MeshBody->SetMaterial(0, ((UCustomGameInstance *)GetWorld()->GetGameInstance())->MaterialDead);
	//MeshBody->SetMaterial(0, MaterialBodyDead); // It causes an exception, when a lot of units are spawned
	// Use a proper death animation

	//Destroy(); // The simplest handling
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

	// ...

	UAbilityMeleeAttack *AbilityMeleeAttack= CreateDefaultSubobject<UAbilityMeleeAttack>(TEXT("MeleeAttack")); // Have no idea, what exactly it does - copying the approach with the camera from dafault code above
	//ma->AddInstanceComponent(UnitDataComponent);
	//ma->RegisterComponent(); // Exception
	//ma->UnitOwner = UnitOwner;
	Abilities.Add(AbilityMeleeAttack); // ??? How to add it to a default array of components? Or simply manually destroy it?

	//UAbilityMeleeAttack *ma = new UAbilityMeleeAttack(_UnitOwner);
}
//-------------------------------------------------------------------------------------------------