// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SaveManager.h"
#include "SB_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSTILLBLUE_API USB_GameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Managers")
	USaveManager* SaveManager;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Save")
	TSubclassOf<USaveManager> SaveManagerClass;

	virtual void Init() override;
};
