// Fill out your copyright notice in the Description page of Project Settings.


#include "SB_NPC.h"

// Sets default values
ASB_NPC::ASB_NPC()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASB_NPC::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASB_NPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASB_NPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

