// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CustomCameraPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class ACustomPlayerController;

UCLASS()
class COMBATTEST_API ACustomCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACustomCameraPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY()
		USceneComponent *SceneComponent;
	UPROPERTY()
		USpringArmComponent *SpringArmComponent;
	UPROPERTY()
		UCameraComponent *CameraComponent;
	
	ACustomPlayerController *PlayerController;
	FVector GetCameraPanDirection();
	
	float Margin;
	float CamSpeed;
	int32 ScreenSizeX;
	int32 ScreenSizeY;
};
