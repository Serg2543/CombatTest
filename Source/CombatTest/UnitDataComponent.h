// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnitDataComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COMBATTEST_API UUnitDataComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "UnitData")
	float HPMax;
	float HP;
	UPROPERTY(EditDefaultsOnly, Category = "UnitData")
	float Damage;

	/*
		class UStatusEffects *StatusEffects; // Status effects can be a separate class, that holds a static set of values for each type of effect, rather than making each instance a separate object.
	*/

	// Sets default values for this component's properties
	UUnitDataComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
