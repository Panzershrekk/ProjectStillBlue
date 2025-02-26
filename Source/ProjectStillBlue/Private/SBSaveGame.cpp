// Fill out your copyright notice in the Description page of Project Settings.


#include "SBSaveGame.h"
#include "Kismet/GameplayStatics.h"

USBSaveGame::USBSaveGame()
{
	/*SaveInfo.SlotNameString = TEXT("TestSaveSlot");
	SaveInfo.UserIndexInt32 = 0;*/
}

void USBSaveGame::AsyncSave(FSaveInfo SaveInfo)
{
	if (USBSaveGame* SaveGameInstance = Cast<USBSaveGame>(UGameplayStatics::CreateSaveGameObject(USBSaveGame::StaticClass())))
	{
		// Set up the (optional) delegate.
		FAsyncSaveGameToSlotDelegate SavedDelegate;
		// USomeUObjectClass::SaveGameDelegateFunction is a void function that takes the following parameters: const FString& SlotName, const int32 UserIndex, bool bSuccess
		//SavedDelegate.BindUObject(SomeUObjectPointer, &USomeUObjectClass::SaveGameDelegateFunction);
		//TODO : Change that in case we need to call the delegate somewhere else
		SavedDelegate.BindUObject(this, &USBSaveGame::SaveCompleteDelegate);

		// Set data on the savegame object.
		//SaveGameInstance->SaveInfo.PlayerName = TEXT("PlayerOne");

		// Start async save process.
		UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, SaveInfo.SlotNameString, SaveInfo.UserIndexInt32, SavedDelegate);
	}
}

void USBSaveGame::AsyncLoad(FString SlotName)
{
	// Set up the delegate.
	FAsyncLoadGameFromSlotDelegate LoadedDelegate;
	// USomeUObjectClass::LoadGameDelegateFunction is a void function that takes the following parameters: const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData
	//LoadedDelegate.BindUObject(SomeUObjectPointer, &USomeUObjectClass::LoadGameDelegateFunction);
	//TODO : Change that in case we need to call the delegate somewhere else
	LoadedDelegate.BindUObject(this, &USBSaveGame::LoadCompleteDelegate);
	UGameplayStatics::AsyncLoadGameFromSlot(SlotName, 0, LoadedDelegate);
}

void USBSaveGame::SaveCompleteDelegate(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{

}

void USBSaveGame::LoadCompleteDelegate(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
{

}