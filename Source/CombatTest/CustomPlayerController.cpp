// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomPlayerController.h"
//#include "Containers/UnrealString.h"
//#include "Blueprint/AIBlueprintHelperLibrary.h"
//#include "Navigation/PathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

#include "CombatTestCharacter.h"
#include "CustomHUD.h"
#include "CustomAIController.h"
#include "CustomGameInstance.h"
#include "Abilities/UnitDataComponentBase.h"

void ACustomPlayerController::LButtonPressed()
{
	if (HUD)
	{
		HUD->InitialPoint = HUD->GetMousePos2D();
		HUD->bStartSelecting = true;
	}
}
//-------------------------------------------------------------------------------------------------

void ACustomPlayerController::LButtonReleased()
{
	if (HUD)
	{
		HUD->bStartSelecting = false;
		SelectedActors = HUD->SelectedActorsDragged;
	}
}
//-------------------------------------------------------------------------------------------------

void ACustomPlayerController::RButtonPressed()
{
	FHitResult HitResult;
	GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult);
	FString s = FString::Printf(TEXT("(%1.1f, %1.1f, %1.1f)"), HitResult.Location.X, HitResult.Location.Y, HitResult.Location.Z);
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, s);
	LastHit = HitResult.Location;

	FVector MoveLocation;
	MoveLocation.Z = HitResult.Location.Z;

	for (int i = 0; i < SelectedActors.Num(); i++)
	{
		MoveLocation.X = HitResult.Location.X + i / 2 * 100;
		MoveLocation.Y = HitResult.Location.Y + i % 2 * 100;
		ACustomAIController* AIController = (ACustomAIController*)SelectedActors[i]->GetController();
		AIController->CommandMoveTo(MoveLocation);
		
		DrawDebugSphere(GetWorld(), MoveLocation, 25, 10, FColor::Red, false, 3);
	}

	/*
	// Old logic, before i removed the unit from selection lists
	// It works, but seems unnecessary
	int n = 0; // Count actual amount, if some actors were skipped
	for (int i = 0; i < SelectedActors.Num(); i++)
	{
		if (!SelectedActors[i]->UnitDataComponent->IsDead())
		{
			MoveLocation.X = HitResult.Location.X + n / 2 * 100;
			MoveLocation.Y = HitResult.Location.Y + n % 2 * 100;
			//UAIBlueprintHelperLibrary::SimpleMoveToLocation(SelectedActors[i]->GetController(), MoveLocation);
			ACustomAIController *AIController = (ACustomAIController*)SelectedActors[i]->GetController();
			AIController->CommandMoveTo(MoveLocation);

			n++;

			DrawDebugSphere(GetWorld(), MoveLocation, 25, 10, FColor::Red, false, 3);
		}
	}
	*/

	/*
	FVector CameraLocation = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
	float Scale = (HitResult.Location - CameraLocation).Size();
	s = FString::Printf(TEXT("distance to camera: %0.1f: "), Scale);
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, s);
	*/
}
//-------------------------------------------------------------------------------------------------

void ACustomPlayerController::CommandStop()
{
	for (int i = 0; i < SelectedActors.Num(); i++)
	{
		ACustomAIController *AIController = (ACustomAIController *)SelectedActors[i]->GetController();
		AIController->CommandStop();
	}
}
//-------------------------------------------------------------------------------------------------

void ACustomPlayerController::CommandHoldPosition()
{
	for (int i = 0; i < SelectedActors.Num(); i++)
	{
		ACustomAIController *AIController = (ACustomAIController *)SelectedActors[i]->GetController();
		AIController->CommandHoldPosition();
	}
}
//-------------------------------------------------------------------------------------------------

void ACustomPlayerController::CommandDisable()
{
	for (int i = 0; i < SelectedActors.Num(); i++)
	{
		ACustomAIController *AIController = (ACustomAIController*)SelectedActors[i]->GetController();
		AIController->CommandDisable();
	}
}
//-------------------------------------------------------------------------------------------------

void ACustomPlayerController::CommandAttackMoveTo()
{
}
//-------------------------------------------------------------------------------------------------

void ACustomPlayerController::SpawnAI(int _Team)
{
	FVector Location = LastHit;
	Location.X += 100;
	// Floor box coordinates: X:(-1660, 1140), Y:(-1900, 1900), Z:(167.5)
	Location.Z = 167.5;
	int i = 0;
	ACombatTestCharacter* Actor = NULL;
	do
	{
		Location.X = FMath::RandRange(-1660, 1140);
		Location.Y = FMath::RandRange(-1900, 1900);

		Actor = (ACombatTestCharacter*)GetWorld()->SpawnActor(((UCustomGameInstance*)GetWorld()->GetGameInstance())->SpawnType, &Location);
		if (Actor != NULL)
		{
			Actor->Team = _Team;
			if (Actor->CompBody) // Set materials to corresponding color
			{

				if (_Team == 0) Actor->CompBody->SetMaterial(0, ((UCustomGameInstance*)GetWorld()->GetGameInstance())->MaterialTeam0);
				else Actor->CompBody->SetMaterial(0, ((UCustomGameInstance*)GetWorld()->GetGameInstance())->MaterialTeam1);
			}
		}
	} while (Actor == NULL && i++ < 5); // Try several times, if random picks unsuitable position
	if (Actor == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Spawn failed");
}
//-------------------------------------------------------------------------------------------------

void ACustomPlayerController::SpawnAI_0()
{
	SpawnAI(0);
}
//-------------------------------------------------------------------------------------------------

void ACustomPlayerController::SpawnAI_1()
{
	SpawnAI(1);
}
//-------------------------------------------------------------------------------------------------

void ACustomPlayerController::BeginPlay()
{
	Super::BeginPlay();

	HUD = Cast<ACustomHUD>(GetHUD());
}
//-------------------------------------------------------------------------------------------------

void ACustomPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("LClick", IE_Pressed, this, &ACustomPlayerController::LButtonPressed);
	InputComponent->BindAction("LClick", IE_Released, this, &ACustomPlayerController::LButtonReleased);

	InputComponent->BindAction("RClick", IE_Pressed, this, &ACustomPlayerController::RButtonPressed);

	InputComponent->BindAction("CommandHoldPosition", IE_Pressed, this, &ACustomPlayerController::CommandHoldPosition);
	InputComponent->BindAction("CommandStop", IE_Pressed, this, &ACustomPlayerController::CommandStop);
	InputComponent->BindAction("CommandDisable", IE_Pressed, this, &ACustomPlayerController::CommandDisable);
	InputComponent->BindAction("CommandAttackMoveTo", IE_Pressed, this, &ACustomPlayerController::CommandAttackMoveTo);

	InputComponent->BindAction("SpawnAI_0", IE_Pressed, this, &ACustomPlayerController::SpawnAI_0);
	InputComponent->BindAction("SpawnAI_1", IE_Pressed, this, &ACustomPlayerController::SpawnAI_1);
}
//-------------------------------------------------------------------------------------------------

ACustomPlayerController::ACustomPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Hand;
}
//-------------------------------------------------------------------------------------------------