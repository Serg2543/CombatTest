// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

typedef class ACombatTestCharacter ABaseCharacterClass; // This is base character class, there should be one in every project. It can have a different different in different projects, so define it in one place;

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"

//#include "Abilities/UnitDataComponentBase.h"

#include "CombatTestCharacter.generated.h"

UCLASS(Blueprintable)
class ACombatTestCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACombatTestCharacter();

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	void SetSelected(bool _Selected);

	// void Attack(ACombatTestCharacter* _Target); // Make an attak against the target


	int Team = -1;
	UMeshComponent *MeshBody = NULL;

	UStaticMeshComponent *WeaponComponent = NULL;
		float WeaponAnimPeriod = 1; // Pseudo-animation of the weapon mesh (rotate the socket)
		float WeaponAnimDir = 1; // Pseudo-animation of the weapon mesh (rotate the socket)
		float WeaponAnimPhase = 0;
	//USkeletalMeshComponent* MeshBody = NULL;
	//UMaterialInstanceDynamic* MaterialBodyTeam0 = NULL, * MaterialBodyTeam1 = NULL, * MaterialBodyDead = NULL;
	UWidgetComponent *WidgetComponent;
	UUserWidget *UserWidget;
	UProgressBar *WidgetHPBar = NULL;
	
	class UUnitDataComponentBase *UnitDataComponent = NULL;
	
	class ACustomAIController* CustomAIController; // Store it, so it doesn't have to be found every tick. Initialize in BeginPlay

protected:
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