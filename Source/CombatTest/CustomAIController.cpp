// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/CrowdFollowingComponent.h"

void ACustomAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ACustomAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Move completed");
}

ACustomAIController::ACustomAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>(TEXT("PathFollowingComponent")))
{
}