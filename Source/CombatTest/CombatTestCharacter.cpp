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

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; // VOODOO: required for MoveTo to work
}

void ACombatTestCharacter::BeginDestroy()
{
	Super::BeginDestroy();
}

void ACombatTestCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsDead)
	{
		CorpseTimer -= DeltaSeconds;
		if (CorpseTimer <= 0) Destroy(); // Can use another phase: fade out, fall through the floor, etc
	}
	else
	{
		/*
		FString s = "";
		if (CustomAIController->GetMoveStatus() == EPathFollowingStatus::Idle) s = "Idle";
		else if (CustomAIController->GetMoveStatus() == EPathFollowingStatus::Moving) s = "Moving";
		else if (CustomAIController->GetMoveStatus() == EPathFollowingStatus::Paused) s = "Paused";
		else if (CustomAIController->GetMoveStatus() == EPathFollowingStatus::Waiting) s = "Waiting";
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, s);
		*/

		if (MoveCommand) // Check, if a manual move order was issued
		{
			// GetCharacterMovement()->IsMovementInProgress() is always false
			if (CustomAIController->GetMoveStatus() == EPathFollowingStatus::Idle) // Check, if destination is reached
			{
				MoveCommand = false; // The unit is idle now
				Target = NULL; // Reset the target, so the closest one can be reacquired
			}
		}

		if (!MoveCommand)
		{
			if (Target && (Target->IsDead || Target->IsPendingKill())) Target = NULL; // Check, if the target is dead in the game or destroyed in the engine
			if (Target == NULL) AcquireTarget();
			if (Target == NULL) // If there is still no target
			{
				// Do idle stuff
				CustomAIController->StopMovement();
			}
			else
			{
				FVector MoveLocation = Target->GetActorLocation();

				float MeleeRange = 200;
				if ((MoveLocation - GetActorLocation()).SizeSquared() < MeleeRange * MeleeRange) // If actors get close, destroy one of the actors
				{
					int coin = FMath::RandRange(0, 1);
					ACombatTestCharacter* UnitToKill;
					if (coin) UnitToKill = Target;
					else UnitToKill = this;
					UnitToKill->Kill();
				}
				else
				{
					CustomAIController->MoveToLocation(MoveLocation);
					/*
					//UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), MoveLocation);
					//CustomAIController->MoveToLocation(MoveLocation, UPathFollowingComponent::DefaultAcceptanceRadius, true, true, false, false, NULL, true);
					
					// It calls OnMoveCompleted each time. In this case, it doesn't matter, but may be a problem at some point.
					// Is it possible to determine the reason of MoveCompleted being called? It has some parameters. Getting to the closest point, if destination is unreachable, or timeout can be important. In other cases, manual check can be good enough.
					// Do not use MoveToActor - sometimes destination will require updates every tick anyway (the unit will want to move not directly towards the target: anticipate and intercept, kite, etc). But in that case, reaching destination will be irrelevant - it will be a custom check anyway?
					*/
				}
			}
		}
	}
}

void ACombatTestCharacter::SetSelected(bool _Selected)
{
	CursorToWorld->SetVisibility(_Selected);
}

void ACombatTestCharacter::Kill()
{
	// Remove the actor from selection lists
	ACustomPlayerController* CustomPlayerController = (ACustomPlayerController*)UGameplayStatics::GetPlayerController(GetWorld(), 0);
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

void ACombatTestCharacter::BeginPlay()
{
	Super::BeginPlay();

	CustomAIController = Cast<ACustomAIController>(this->GetController());
	if (CustomAIController == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "CustomAIController == NULL");

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

void ACombatTestCharacter::AcquireTarget()
{
	TArray<AActor*> OutActors;
	TArray< AActor* > ActorsToIgnore;
	ActorsToIgnore.Add(this);

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACombatTestCharacter::StaticClass(), OutActors); // Bruteforce approach

	if (OutActors.Num() == 0) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Find actors - no results");
	Target = NULL;
	float DistMinSquared = AcquisitionRange * AcquisitionRange;
	float DistCurSquared;
	FVector pos = GetActorLocation();
	FVector dest;

	for (int i = 0; i < OutActors.Num(); i++)
	{
		if (!((ACombatTestCharacter*)OutActors[i])->IsDead && ((ACombatTestCharacter*)OutActors[i])->Team != Team)
		{
			dest = ((ACombatTestCharacter*)OutActors[i])->GetActorLocation();
			if ((DistCurSquared = (dest - pos).SizeSquared()) < DistMinSquared)
			{
				Target = (ACombatTestCharacter*)OutActors[i];
				DistMinSquared = DistCurSquared;
			}
		}
	}
}