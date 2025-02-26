// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SBSaveGame.h"
#include "SaveManager.generated.h"

class USB_GameInstance;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSaveGameLoaded, USBSaveGame*, SaveGameData);

UCLASS()
class PROJECTSTILLBLUE_API USaveManager : public UObject
{
	GENERATED_BODY()
public:
    void Initialize(USB_GameInstance* Instance);

    UPROPERTY(BlueprintReadOnly, Category = "Manager")
    USB_GameInstance* SBGameInstance;

    UFUNCTION(BlueprintCallable, Category = "SaveHandling")
    void AsyncSave(FSaveInfo SaveInfo);

    UFUNCTION(BlueprintCallable, Category = "SaveHandling")
    void AsyncLoad(FString SlotName);

    void SaveCompleteDelegate(const FString& SlotName, const int32 UserIndex, bool bSuccess);
    void LoadCompleteDelegate(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);

    // Stocker le dernier fichier de sauvegarde chargé
    UPROPERTY(BlueprintReadOnly, Category = "SaveHandling")
    USBSaveGame* LoadedSaveGame;

    UPROPERTY(BlueprintAssignable, Category = "SaveHandling")
    FOnSaveGameLoaded OnSaveGameLoaded;
};
