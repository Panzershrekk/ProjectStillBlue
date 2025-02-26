// Fill out your copyright notice in the Description page of Project Settings.


#include "SBSaveGame.h"
#include "Kismet/GameplayStatics.h"

USBSaveGame::USBSaveGame()
{
	SaveInfo.SlotNameString = TEXT("TestSaveSlot");
	SaveInfo.UserIndexInt32 = 0;
}

