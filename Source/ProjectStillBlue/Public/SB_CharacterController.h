// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "Logging/LogMacros.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "SB_CharacterController.generated.h"

class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class PROJECTSTILLBLUE_API ASB_CharacterController : public ACharacter
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	virtual void NotifyControllerChanged() override;

public:
	// Sets default values for this character's properties
	ASB_CharacterController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;	

	//UFUNCTION(BlueprintCallable, Category = "Test")
};
