// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SB_NPC.generated.h"

UCLASS()
class PROJECTSTILLBLUE_API ASB_NPC : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASB_NPC();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
