// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "Logging/LogMacros.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "SB_CharacterMovementComponent.h"
#include "SB_CharacterController.generated.h"

class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class PROJECTSTILLBLUE_API ASB_CharacterController : public ACharacter
{
	GENERATED_BODY()

	// properties

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RightWaveTriggerAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LeftWaveTriggerAction;
	// variables
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SurfMovement, meta = (AllowPrivateAccess = "true"))
	float MaxSurfSpeed = 5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SurfMovement, meta = (AllowPrivateAccess = "true"))
	float SurfAcceleration = 1;

	FVector Velocity;

	virtual void NotifyControllerChanged() override;
	void AddAccelerationInput(FVector WorldDirection, float ScaleValue);

public:
	// Sets default values for this character's properties
	ASB_CharacterController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Look(const FInputActionValue& Value);

	void Move(const FInputActionValue& Value);

	USB_CharacterMovementComponent* GetUSBCharacterMovementComponent();

	void ProcessRotation(FRotator Rotation);

	void RightWaveTrigger(const FInputActionValue& Value);

	void LeftWaveTrigger(const FInputActionValue& Value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;	

	//UFUNCTION(BlueprintCallable, Category = "Test")
};
