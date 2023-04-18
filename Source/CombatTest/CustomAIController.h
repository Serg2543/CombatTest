// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "CustomAIController.generated.h"

/**
 * 
 */
UCLASS()
class COMBATTEST_API ACustomAIController : public AAIController
{
	GENERATED_BODY()
	protected:
		virtual void Tick(float DeltaSeconds) override;
		void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;
	public:
		ACustomAIController(const FObjectInitializer& ObjectInitializer);
};
