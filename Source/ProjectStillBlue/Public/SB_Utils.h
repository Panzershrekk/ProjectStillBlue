// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SB_Utils.generated.h"

/**
 *
 */

USTRUCT(BlueprintType)
struct FSaveInfo
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FString PlayerName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	FString SlotNameString;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save")
	int32 UserIndexInt32;
};