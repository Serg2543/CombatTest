// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CustomGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class COMBATTEST_API UCustomGameInstance : public UGameInstance
{
	GENERATED_BODY()
	public:
		UPROPERTY(EditAnywhere)
			TSubclassOf<class ACombatTestCharacter> SpawnType;
		UPROPERTY(EditAnywhere)
			UMaterialInterface *MaterialTeam0 = NULL;
		UPROPERTY(EditAnywhere)
			UMaterialInterface *MaterialTeam1 = NULL;
		UPROPERTY(EditAnywhere)
			UMaterialInterface *MaterialDead = NULL;

		UCustomGameInstance();
};
