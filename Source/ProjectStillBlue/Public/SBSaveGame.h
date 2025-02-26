// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SBSaveGame.generated.h"

/**
 * https://dev.epicgames.com/documentation/en-us/unreal-engine/saving-and-loading-your-game-in-unreal-engine
 */
UCLASS()
class PROJECTSTILLBLUE_API USBSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	/*UPROPERTY(VisibleAnywhere, Category = Save)
	FSaveInfo SaveInfo;*/


	USBSaveGame();
	void AsyncSave(FSaveInfo SaveInfo);
	void AsyncLoad(FString SlotName);

	void SaveCompleteDelegate(const FString& SlotName, const int32 UserIndex, bool bSuccess);
	void LoadCompleteDelegate(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);
};

USTRUCT(BlueprintType)
struct FSaveInfo
{
	GENERATED_BODY()
	UPROPERTY(VisibleAnywhere, Category = Save)
	FString PlayerName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FString SlotNameString;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	int32 UserIndexInt32;
};