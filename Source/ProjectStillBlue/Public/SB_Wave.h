// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SB_Wave.generated.h"

UCLASS()
class PROJECTSTILLBLUE_API ASB_Wave : public AActor
{
	GENERATED_BODY()
	
public:
    ASB_Wave();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* WaveMesh;

};
