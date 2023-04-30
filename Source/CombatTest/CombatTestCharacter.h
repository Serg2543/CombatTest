// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "CombatTestCharacter.generated.h"

UCLASS(Blueprintable)
class ACombatTestCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACombatTestCharacter();

	// Called every frame.

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	void SetSelected(bool _Selected);

	bool IsDead = false;
	float CorpseTimer = 3; // Time before the corpse is despawned
	void Kill();

	// void Attack(ACombatTestCharacter* _Target); // Make an attak against the target


	int Team = -1;
	USkeletalMeshComponent* MeshBody = NULL;
	UMaterialInstanceDynamic* MaterialBodyTeam0 = NULL, *MaterialBodyTeam1 = NULL, *MaterialBodyDead = NULL;

	/*
		UUnitData *UnitData;
			Skills and other leveling data
			Stats (HP, damage and other derived stats)
			UWeapon *Weapon;
	*/
	

protected:
	class ACustomAIController *CustomAIController; // Store it, so it doesn't have to be found every tick. Initialize in BeginPlay
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UDecalComponent* CursorToWorld;
};

