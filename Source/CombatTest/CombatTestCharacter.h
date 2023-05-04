// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CombatTestCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UDecalComponent;
class UStaticMeshComponent;
class UWidgetComponent;
class UUserWidget;
class UProgressBar;

class ACustomAIController;
class UUnitDataComponentBase;

UCLASS(Blueprintable)
class ACombatTestCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACombatTestCharacter();

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	void SetSelected(bool _Selected);

	int Team = -1;
	UMeshComponent *CompBody = NULL;
	UStaticMeshComponent *CompHead = NULL;
	UStaticMeshComponent *CompFoot_L = NULL;
	UStaticMeshComponent *CompFoot_R = NULL;
		FVector Foot_L_BaseLoc;
		FVector Foot_R_BaseLoc;
		float MoveAnimPeriod = 1;
		float MoveAnimTime = 0;
	UStaticMeshComponent* CompHand_R = NULL;
	UStaticMeshComponent *CompHand_L = NULL;
	//USkeletalMeshComponent* MeshBody = NULL;
	//UMaterialInstanceDynamic* MaterialBodyTeam0 = NULL, * MaterialBodyTeam1 = NULL, * MaterialBodyDead = NULL;
	UWidgetComponent *WidgetComponent;
	UUserWidget *UserWidget;
	UProgressBar *WidgetHPBar = NULL;
	
	ACustomAIController* CustomAIController; // Store it, so it doesn't have to be found every tick. Initialize in BeginPlay
	UUnitDataComponentBase *UnitDataComponent = NULL;

protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UDecalComponent* CursorToWorld;
};