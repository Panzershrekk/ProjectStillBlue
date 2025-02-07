// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SB_CharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSTILLBLUE_API USB_CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	
public :
	void AddRotation(FRotator Rotation);
};
