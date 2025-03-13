// Fill out your copyright notice in the Description page of Project Settings.


#include "SB_Wave.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "SB_CharacterController.h"

ASB_Wave::ASB_Wave()
{
    PrimaryActorTick.bCanEverTick = true;


    WaveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WaveMesh"));
    RootComponent = WaveMesh;
}

void ASB_Wave::BeginPlay()
{
    Super::BeginPlay();
    SetLifeSpan(5.0f);
}

void ASB_Wave::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Faire avancer la vague légèrement
    FVector NewLocation = GetActorLocation() + GetActorForwardVector() * 200.f * DeltaTime;
    SetActorLocation(NewLocation);
}
