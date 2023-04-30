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

#include "Materials/Material.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "CustomAIController.h"
#include "CustomHUD.h"
#include "CustomPlayerController.h"

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

	if (IsDead)
	{
		CorpseTimer -= DeltaSeconds;
		if (CorpseTimer <= 0) Destroy(); // Can use another phase: fade out, fall through the floor, etc
	}
}
//-------------------------------------------------------------------------------------------------

void ACombatTestCharacter::SetSelected(bool _Selected)
{
	CursorToWorld->SetVisibility(_Selected);
}
//-------------------------------------------------------------------------------------------------

void ACombatTestCharacter::Kill()
{
	// Remove the actor from selection lists
	ACustomPlayerController *CustomPlayerController = (ACustomPlayerController *)UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ACustomHUD* CustomHUD = (ACustomHUD*)CustomPlayerController->GetHUD();
	CustomPlayerController->SelectedActors.Remove(this);
	CustomHUD->SelectedActorsDragged.Remove(this);

	// Disable AI
	CustomAIController->StopMovement();
	CustomAIController->UnPossess();
	CustomAIController->Destroy(); // Destroy the controller explicitly, because it it is not automatically destroyed after Unpossess()
	IsDead = true;
	// Turn off collision with units
	SetActorEnableCollision(false);
	// Change visual appearance
	MeshBody->SetMaterial(0, MaterialBodyDead); // It causes an exception, when a lot of units are spawned
	// Use a proper death animation

	//Destroy(); // The simplest handling
}
//-------------------------------------------------------------------------------------------------

void ACombatTestCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Store AIController, so it doesn't have to be retreived every time
	CustomAIController = Cast<ACustomAIController>(this->GetController());
	if (CustomAIController == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "CustomAIController == NULL");

	UnitDataComponent->SetOwner(this);

	// Test attaching a component to graybox unit
	/*
			!!! Do in BPs - it seems easier and won't clutter the code
				Literally move the component in the hierarchy and select the socket
	*/
	/*
	UCapsuleComponent *cap = FindComponentByClass<UCapsuleComponent>();
	TArray<USceneComponent *> comp;
	cap->GetChildrenComponents(false, comp);
	UStaticMesh *Weapon = NULL;
	UStaticMeshComponent *meshcomp = NULL;
	FString s;
	
	for (int i = 0; i < comp.Num(); i++)
	{
		s = FString::Printf(TEXT("%i: "), i);
		meshcomp = Cast<UStaticMeshComponent>(comp[i]); // Direct indexing
		if (meshcomp == NULL) s += "UStaticMeshComponent == NULL";
		else
		{
			if (meshcomp->GetSocketByName(TEXT("Weapon")) == NULL) s += "Socket == NULL";
			//if (meshcomp->HasAnySockets()) s += "Socket == NULL";
		}
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, s);
	}	

	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::SnapToTarget, false);
	if (!comp[5]->AttachToComponent(comp[4], AttachmentTransformRules, TEXT("Weapon"))) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "AttachToComponent == NULL");
	*/

	// Initialize materials for team colors
	/*
	// Tutorial for dynamic materials: https://www.youtube.com/watch?v=AsAx9HObtIY
	// FIX: Don't have to be in each instance of an actor, use GameInstance
	// Had an exception in KIll() {... MeshBody->SetMaterial(0, MaterialBodyDead); ...} after spawning a lot of unit
	// https://couchlearn.com/how-to-use-the-game-instance-in-unreal-engine-4/
	*/

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
}
//-------------------------------------------------------------------------------------------------

void ACombatTestCharacter::BeginDestroy()
{
	Super::BeginDestroy();
}
//-------------------------------------------------------------------------------------------------