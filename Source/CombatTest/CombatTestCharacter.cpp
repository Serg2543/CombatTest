// Copyright Epic Games, Inc. All Rights Reserved.

#include "CombatTestCharacter.h"

#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Engine/World.h"

//#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "CustomAIController.h"
#include "CustomHUD.h"
#include "CustomPlayerController.h"
#include "CustomGameInstance.h"
#include "Abilities/UnitDataComponentBase.h"
#include "Abilities/AbilityMeleeAttack.h"

ACombatTestCharacter::ACombatTestCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
	CursorToWorld->SetVisibility(false);

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Initializae UitData component
	UnitDataComponent = CreateDefaultSubobject<UUnitDataComponentBase>(TEXT("UnitData"));
	//UnitDataComponent->RegisterComponent();//ma->RegisterComponent(); // Exception
	//UnitDataComponent->UnitOwner = this;
	
	//UnitDataComponent->SetupAttachment(RootComponent);
	AddInstanceComponent(UnitDataComponent);
	UnitDataComponent->SetOwner(this);
}
//-------------------------------------------------------------------------------------------------

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

	
	// Pseudo-animate the weapon
	float t = UnitDataComponent->Abilities[0]->TimeCounter;
	float p = UnitDataComponent->Abilities[0]->Predelay;
	float d = UnitDataComponent->Abilities[0]->TotalTime;
	float x = 0; // Phase of predelay or backswing
	if (!UnitDataComponent->bBusy)
	{
		x = 0.5; // Default pose is with the weapon at an angle - it looks natural. There is a jump on the very 1st attack, but it is not important
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
	WeaponComponent->SetRelativeRotation(rot);
	
		// Other options
		//SetRelativeRotation
		//SetRelativeRotation_Direct
		//SetRelativeRotationExact
}
//-------------------------------------------------------------------------------------------------

void ACombatTestCharacter::SetSelected(bool _Selected)
{
	CursorToWorld->SetVisibility(_Selected);
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
			if (meshcomp->GetSocketByName(TEXT("Weapon")) != NULL)
			{
				MeshBody = meshcomp;
				MeshBody->GetChildrenComponents(false, comp2);
				if (comp2.Num() > 0) WeaponComponent = Cast<UStaticMeshComponent>(comp2[0]);
			}
		}
	}
	
	if (MeshBody == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "MeshBody == NULL");
	if (WeaponComponent == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "WeaponComponent == NULL");

	// Initialize materials for team colors
	/*
	// Tutorial for dynamic materials: https://www.youtube.com/watch?v=AsAx9HObtIY
	// FIX: Don't have to be in each instance of an actor, use GameInstance
	// Had an exception in KIll() {... MeshBody->SetMaterial(0, MaterialBodyDead); ...} after spawning a lot of unit
	// https://couchlearn.com/how-to-use-the-game-instance-in-unreal-engine-4/
	*/
	/*
	MeshBody = FindComponentByClass<USkeletalMeshComponent>();
	if (MeshBody == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Mesh == NULL");
	else
	{
		UMaterialInterface* Mat = MeshBody->GetMaterial(0);
		if (Mat == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Static material == NULL");
		else
		{
			MaterialBodyTeam0 = UMaterialInstanceDynamic::Create(MeshBody->GetMaterial(0), NULL);
			if (MaterialBodyTeam0 == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "MaterialBodyTeam0 == NULL");
			else MaterialBodyTeam0->SetVectorParameterValue(FName(TEXT("BodyColor")), FLinearColor(0.9f, 0.1f, 0.1f));

			MaterialBodyTeam1 = UMaterialInstanceDynamic::Create(MeshBody->GetMaterial(0), NULL);
			if (MaterialBodyTeam1 == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "MaterialBodyTeam1 == NULL");
			else MaterialBodyTeam1->SetVectorParameterValue(FName(TEXT("BodyColor")), FLinearColor(0.1f, 0.1f, 0.9f));
			
			MaterialBodyDead = UMaterialInstanceDynamic::Create(MeshBody->GetMaterial(0), NULL);
			if (MaterialBodyDead == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "MaterialBodyDead == NULL");
			else MaterialBodyDead->SetVectorParameterValue(FName(TEXT("BodyColor")), FLinearColor(0.4f, 0.4f, 0.0f));
		}
	}
	*/
}
//-------------------------------------------------------------------------------------------------

void ACombatTestCharacter::BeginDestroy()
{
	Super::BeginDestroy();
}
//-------------------------------------------------------------------------------------------------