// Fill out your copyright notice in the Description page of Project Settings.


#include "SB_GameInstance.h"

void USB_GameInstance::Init()
{
    Super::Init();
    if (SaveManagerClass)
    {
        SaveManager = NewObject<USaveManager>(this, SaveManagerClass);
        if (SaveManager)
        {
            SaveManager->Initialize(this);
        }
    }
}