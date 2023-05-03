// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CustomGameInstance.generated.h"

class ACombatTestCharacter;

UCLASS()
class COMBATTEST_API UCustomGameInstance : public UGameInstance
{
	GENERATED_BODY()
	public:
		UPROPERTY(EditAnywhere)
			TSubclassOf<ACombatTestCharacter> SpawnType;
		UPROPERTY(EditAnywhere)
			UMaterialInterface *MaterialTeam0 = NULL;
		UPROPERTY(EditAnywhere)
			UMaterialInterface *MaterialTeam1 = NULL;
		UPROPERTY(EditAnywhere)
			UMaterialInterface *MaterialDead = NULL;

		UCustomGameInstance();
};
