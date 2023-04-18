// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomPlayerController.h"
#include "CustomGameMode.h"
#include "Containers/UnrealString.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Navigation/PathFollowingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

#include "CustomAIController.h"
#include "CustomGameInstance.h"

ACustomPlayerController::ACustomPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Hand;
}

void ACustomPlayerController::BeginPlay()
{
	Super::BeginPlay();

	HUD = Cast<ACustomHUD>(GetHUD());

	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(GetWorld());
	if (GameModeBase == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "GameModeBase == NULL");
	else
	{
		GameMode = Cast<ACustomGameMode >(GameModeBase);
		if (GameMode == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "GameMode cast failed");
	}
}

void ACustomPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("LClick", IE_Pressed, this, &ACustomPlayerController::SelectionPressed);
	InputComponent->BindAction("LClick", IE_Released, this, &ACustomPlayerController::SelectionReleased);

	InputComponent->BindAction("RClick", IE_Released, this, &ACustomPlayerController::MoveReleased);

	InputComponent->BindAction("SpawnAI_0", IE_Pressed, this, &ACustomPlayerController::SpawnAI_0);
	InputComponent->BindAction("SpawnAI_1", IE_Pressed, this, &ACustomPlayerController::SpawnAI_1);
}

void ACustomPlayerController::SelectionPressed()
{
	if (HUD)
	{
		HUD->InitialPoint = HUD->GetMousePos2D();
		HUD->bStartSelecting = true;
	}
}

void ACustomPlayerController::SelectionReleased()
{
	if (HUD)
	{
		HUD->bStartSelecting = false;
		SelectedActors = HUD->SelectedActorsDragged;
	}
}

void ACustomPlayerController::MoveReleased()
{
	FHitResult HitResult;
	GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult);
	FString s = FString::Printf(TEXT("(%1.1f, %1.1f, %1.1f)"), HitResult.Location.X, HitResult.Location.Y, HitResult.Location.Z);
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, s);
	LastHit = HitResult.Location;

	FVector MoveLocation;
	MoveLocation.Z = HitResult.Location.Z;

	int n = 0; // Count actual amount, if some actors were skipped
	for (int i = 0; i < SelectedActors.Num(); i++)
	{
		if (!SelectedActors[i]->IsDead)
		{
			MoveLocation.X = HitResult.Location.X + n / 2 * 100;
			MoveLocation.Y = HitResult.Location.Y + n % 2 * 100;
			//UAIBlueprintHelperLibrary::SimpleMoveToLocation(SelectedActors[i]->GetController(), MoveLocation);
			ACustomAIController* AIController = (ACustomAIController*)SelectedActors[i]->GetController();
			AIController->MoveToLocation(MoveLocation, UPathFollowingComponent::DefaultAcceptanceRadius, true, true, false, false, NULL, true);
			SelectedActors[i]->MoveCommand = true;

			n++;

			DrawDebugSphere(GetWorld(), MoveLocation, 25, 10, FColor::Red, false, 3);
		}
	}
}

void ACustomPlayerController::SpawnAI(int _Team)
{
	if (GameMode)
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

			Actor = (ACombatTestCharacter *)GetWorld()->SpawnActor(((UCustomGameInstance*)GetWorld()->GetGameInstance())->SpawnType, &Location);
			if (Actor != NULL)
			{
				Actor->Team = _Team;
				if (Actor->MeshBody) // Set materials to corresponding color
				{
					if (_Team == 0) Actor->MeshBody->SetMaterial(0, Actor->MaterialBodyTeam0);
					else Actor->MeshBody->SetMaterial(0, Actor->MaterialBodyTeam1);
				}
			}
		} while (Actor == NULL && i++ < 5); // Try several times, if random picks unsuitable position
		if (Actor == NULL) GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Spawn failed");
	}
}


void ACustomPlayerController::SpawnAI_0()
{
	SpawnAI(0);
}

void ACustomPlayerController::SpawnAI_1()
{
	SpawnAI(1);
}