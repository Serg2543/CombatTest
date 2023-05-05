// Copyright Epic Games, Inc. All Rights Reserved.

#include "CombatTestCharacter.h"

//#include "UObject/ConstructorHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
//#include "GameFramework/PlayerController.h"
//#include "Engine/World.h"

//#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
//#include "Kismet/KismetMathLibrary.h"

#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"

#include "CustomAIController.h"
#include "CustomHUD.h"
#include "CustomPlayerController.h"
#include "CustomGameInstance.h"
#include "Abilities/UnitDataComponentBase.h"
#include "Abilities/AbilityMeleeAttack.h"

void ACombatTestCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	/*
	// Make the widget face the camera - it is not what it has to be
	FVector CameraLocation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
	FRotator rot = UKismetMathLibrary::FindLookAtRotation(WidgetComponent->GetComponentToWorld().GetLocation(), CameraLocation);
	WidgetComponent->SetWorldRotation(rot);
	*/
	/*
	// Doesn't work, even in the editor
	FVector Scale = FVector(3, 3, 30);
	WidgetComponent->SetWorldScale3D(Scale);
	*/

	if (WidgetHPBar)
	{
		float Percent = UnitDataComponent->HP / UnitDataComponent->HPMax;
		if (Percent <= 0) Percent = 0;
		WidgetHPBar->SetPercent(Percent);

		// Scale by distance to the camera. Not precise scaling, but good enough.
		FVector CameraLocation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
		FVector WidgetLocation = WidgetComponent->GetComponentToWorld().GetLocation();

		float Scale = 1650 / (WidgetLocation - CameraLocation).Size(); // Using empirical reference value
		if (Scale > 1) Scale = 1;
		FVector2D Scale2D = FVector2D(Scale, Scale);
		UserWidget->SetRenderScale(Scale2D);
		const float ScaleVisibilityThreshold = 0.25; // tmp
		WidgetHPBar->SetVisibility(!UnitDataComponent->IsDead() && WidgetHPBar->Percent < 1 && Scale > ScaleVisibilityThreshold ? ESlateVisibility::Visible : ESlateVisibility::Hidden); // Show only if < 100%
	}
	else GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "WidgetHPBar == NULL");

	// Pseudo-animation
		// Weapon
		float t = UnitDataComponent->Abilities[0]->TimeCounter;
		float p = UnitDataComponent->Abilities[0]->Predelay;
		float d = UnitDataComponent->Abilities[0]->TotalTime;
		float x = 0; // Phase of predelay or backswing
		if (!UnitDataComponent->bBusy)
		{
			//x = 0.5; // Default pose is with the weapon at an angle - it looks natural. There is a jump on the very 1st attack, but it is not important
		}
		else // If an attack is in progress, calculate the phase and corresponding rotation
		{
			if (t < p) x = t / p; // Predelay
			else x = 1 - (t - p) / (d - p); // Recovery, in reverse direction
			if (x < 0) x = 0;
			else if (x > 1) x = 1;
		}
		//x = 0.5;
		//x = t / l;
	
		FRotator rot = FRotator(-90 * x, 0, 0);
		//WeaponComponent->SetRelativeRotation_Direct(rot);
		CompHand_R->SetRelativeRotation(rot);

		// Feet
		// Keep moving, if the character is moving or current offset is non-zero, so it eventually gets to 0
		x = sin(2 * 3.14159265 * MoveAnimTime / MoveAnimPeriod);
		float SnapThrreshold = 1.1 * sin(2 * 3.14159265 * DeltaSeconds / MoveAnimPeriod);
		if (x > SnapThrreshold || (CustomAIController && CustomAIController->GetMoveStatus() != EPathFollowingStatus::Idle))
		{
			MoveAnimTime += DeltaSeconds;
		}
		else
		{
			MoveAnimTime = 0;
			x = 0;
		}
		const float Magnitude = 10;
		CompFoot_L->SetRelativeLocation(Foot_L_BaseLoc + FVector(0, 0, -Magnitude * x));
		CompFoot_R->SetRelativeLocation(Foot_R_BaseLoc + FVector(0, 0,  Magnitude * x));
}
//-------------------------------------------------------------------------------------------------

void ACombatTestCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Store AIController, so it doesn't have to be retreived every time
	CustomAIController = Cast<ACustomAIController>(this->GetController());
	if (CustomAIController == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "CustomAIController == NULL");

	// Store WidgetComponent for HP bar, so it doesn't have to be retreived every time
	WidgetComponent = FindComponentByClass<UWidgetComponent>();
	if (WidgetComponent == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "WidgetComponent == NULL");
	else
	{
		UserWidget = WidgetComponent->GetUserWidgetObject();
		if (UserWidget == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "UserWidget == NULL");
		else
		{
			WidgetHPBar = (UProgressBar*)UserWidget->GetWidgetFromName(TEXT("PB_HPBar"));
			if (WidgetHPBar == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "WidgetHPBar == NULL");
			else WidgetHPBar->SetVisibility(ESlateVisibility::Hidden); // Hide by default at full HP
		}
	}
	// Find the capsule mesh, that represents the body
	// THere should be a better way
	UCapsuleComponent* cap = FindComponentByClass<UCapsuleComponent>();
	TArray<USceneComponent*> comp; 
	TArray<USceneComponent*> comp2;
	cap->GetChildrenComponents(false, comp);
	
	UStaticMeshComponent *meshcomp = NULL; // The "body" is the mesh with corresponding sockets - find it and store for future reference
	for (int i = 0; i < comp.Num(); i++)
	{
		meshcomp = Cast<UStaticMeshComponent>(comp[i]);
		if (meshcomp != NULL)
		{
			if (meshcomp->HasAnySockets())
			{
				CompBody = meshcomp;
				CompBody->GetChildrenComponents(false, comp2);
				if (comp2.Num() > 0)
				{
					CompHead = Cast<UStaticMeshComponent>(comp2[0]);
					CompFoot_L = Cast<UStaticMeshComponent>(comp2[1]);
					CompFoot_R = Cast<UStaticMeshComponent>(comp2[2]);
					CompHand_L = Cast<UStaticMeshComponent>(comp2[3]);
					CompHand_R = Cast<UStaticMeshComponent>(comp2[4]);

					Foot_L_BaseLoc = CompFoot_L->GetRelativeLocation();
					Foot_R_BaseLoc = CompFoot_R->GetRelativeLocation();
				}
			}
			else // FIX: For now, the only other mesh is selection circle, but it is lazy
			{
				SelectionCircle = meshcomp;
			}
		}
	}
	
	if (CompBody == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "CompBody == NULL");
	if (CompHead == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "CompHead == NULL");
	if (CompFoot_L == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "CompFoot_L == NULL");
	if (CompFoot_R == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "CompFoot_R == NULL");
	if (CompHand_L == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "CompHand_L == NULL");
	if (CompHand_R == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "CompHand_R == NULL");

	// Initialize materials for team colors
}
//-------------------------------------------------------------------------------------------------

void ACombatTestCharacter::BeginDestroy()
{
	Super::BeginDestroy();
}
//-------------------------------------------------------------------------------------------------

void ACombatTestCharacter::SetSelected(bool _Selected)
{
	SelectionCircle->SetVisibility(_Selected);
}
//-------------------------------------------------------------------------------------------------

ACombatTestCharacter::ACombatTestCharacter()
{
	// From the template
	// Set size for player capsule
	//GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	// Custom code
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Initializae UitData component
	UnitDataComponent = CreateDefaultSubobject<UUnitDataComponentBase>(TEXT("UnitData"));
	//UnitDataComponent->RegisterComponent();//ma->RegisterComponent(); // Exception
	//UnitDataComponent->UnitOwner = this;

	//UnitDataComponent->SetupAttachment(RootComponent);
	AddInstanceComponent(UnitDataComponent);
	UnitDataComponent->SetOwner(this);

	/*
	// Tried to load dynamically, like the decal in the template example, but it seems to not work
	// Add #include "UObject/ConstructorHelpers.h"
	SelectionCircle = CreateDefaultSubobject<UStaticMeshComponent>("SelectionCircle");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SelectionCircleStaticMesh(TEXT("StaticMesh'/Game/TopDownCPP/GrayboxCharacter/Selection_GB.Selection_GB'"));
	if (SelectionCircleStaticMesh.Succeeded())
	{
		SelectionCircle->SetStaticMesh(SelectionCircleStaticMesh.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterial> SelectionCircleMaterial(TEXT("Material'/Game/TopDownCPP/GrayboxCharacter/Materials/M_Selection.M_Selection'"));
	if (SelectionCircleMaterial.Succeeded())
	{
		SelectionCircle->SetMaterial(0, SelectionCircleMaterial.Object);
	}
	*/
}
//-------------------------------------------------------------------------------------------------