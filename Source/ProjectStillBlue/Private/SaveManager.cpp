// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveManager.h"
#include "Kismet/GameplayStatics.h"

void USaveManager::Initialize(USB_GameInstance* Instance)
{
    SBGameInstance = Instance;
}

void USaveManager::AsyncSave(FSaveInfo SaveInfo)
{
	if (USBSaveGame* SaveGameInstance = Cast<USBSaveGame>(UGameplayStatics::CreateSaveGameObject(USBSaveGame::StaticClass())))
	{
		// Set up the (optional) delegate.
		FAsyncSaveGameToSlotDelegate SavedDelegate;
		// USomeUObjectClass::SaveGameDelegateFunction is a void function that takes the following parameters: const FString& SlotName, const int32 UserIndex, bool bSuccess
		//SavedDelegate.BindUObject(SomeUObjectPointer, &USomeUObjectClass::SaveGameDelegateFunction);
		//TODO : Change that in case we need to call the delegate somewhere else
		SavedDelegate.BindUObject(this, &USaveManager::SaveCompleteDelegate);
		// Start async save process.
		UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, SaveInfo.SlotNameString, SaveInfo.UserIndexInt32, SavedDelegate);
	}
}

void USaveManager::AsyncLoad(FString SlotName)
{
	// Set up the delegate.
	FAsyncLoadGameFromSlotDelegate LoadedDelegate;
	// USomeUObjectClass::LoadGameDelegateFunction is a void function that takes the following parameters: const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData
	//LoadedDelegate.BindUObject(SomeUObjectPointer, &USomeUObjectClass::LoadGameDelegateFunction);
	//TODO : Change that in case we need to call the delegate somewhere else
	LoadedDelegate.BindUObject(this, &USaveManager::LoadCompleteDelegate);
	UGameplayStatics::AsyncLoadGameFromSlot(SlotName, 0, LoadedDelegate);
}

void USaveManager::SaveCompleteDelegate(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
	UE_LOG(LogTemp, Warning, TEXT("Save Success"));
}

void USaveManager::LoadCompleteDelegate(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
{
	if (LoadedGameData)
	{
		LoadedSaveGame = Cast<USBSaveGame>(LoadedGameData);
		if (LoadedSaveGame)
		{
			OnSaveGameLoaded.Broadcast(LoadedSaveGame);
		}
	}
}