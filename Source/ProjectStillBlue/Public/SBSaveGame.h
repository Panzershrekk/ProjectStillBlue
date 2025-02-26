// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SB_Utils.h"
#include "SBSaveGame.generated.h"

/**
 * https://dev.epicgames.com/documentation/en-us/unreal-engine/saving-and-loading-your-game-in-unreal-engine
 */
UCLASS()
class PROJECTSTILLBLUE_API USBSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Save")
	FSaveInfo SaveInfo;
	USBSaveGame();
};