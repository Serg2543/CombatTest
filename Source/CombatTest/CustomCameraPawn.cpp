// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCameraPawn.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

#include "CustomPlayerController.h"

ACustomCameraPawn::ACustomCameraPawn()
{
	Margin = 15;
	CamSpeed = 50;

 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent; // That is what was in the tutorial https://youtu.be/6JAuSV4WZuc?t=189
	//SetRootComponent(SceneComponent); // Is this the same?
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(SceneComponent);
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->TargetArmLength = 0;
	// Try "Enable camera lag" https://youtu.be/0MWuiS5c1W4?list=PLPpgDoSBYYWhuhvwul_p4H64LPu2asU3l&t=40

	//SpringArmComponent->SetRelativeRotation(FRotator(-50, 0, 0));
		// Set rotation in the editor - it seems to add up
	// starting location (-1800, 0, 2500)
	// starting rotation (0, -60, 0)
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
}

// Called when the game starts or when spawned
void ACustomCameraPawn::BeginPlay()
{
	Super::BeginPlay();
	PlayerController = Cast<ACustomPlayerController>(GetController());
	PlayerController->GetViewportSize(ScreenSizeX, ScreenSizeY);
}

// Called every frame
void ACustomCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*
	FVector v = GetCameraPanDirection();
	if (v != FVector::ZeroVector)
	{
		AddActorWorldOffset(CamSpeed * v);
	}
	*/

	// If camera is turned, have to use fancier math
	FVector v = GetCameraPanDirection();
	FVector r = GetActorRightVector();
	FVector f = GetActorForwardVector();
	f.Z = 0;
	f = f.GetSafeNormal();
	if (v != FVector::ZeroVector)
	{
		AddActorWorldOffset(CamSpeed * (v.X * f + v.Y * r));
	}	
}

// Called to bind functionality to input
void ACustomCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FVector ACustomCameraPawn::GetCameraPanDirection()
{
	float X, Y;
	float CamDirX = 0;
	float CamDirY = 0;

	PlayerController->GetMousePosition(X, Y);
	if (X <= Margin) CamDirY = -1;
	else if (X >= ScreenSizeX - Margin - 1) CamDirY = 1;
	if (Y <= Margin) CamDirX = 1;
	else if (Y >= ScreenSizeY - Margin - 1) CamDirX = -1;

	return FVector(CamDirX, CamDirY, 0);
}