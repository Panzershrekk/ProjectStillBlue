// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "Logging/LogMacros.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "SB_CharacterMovementComponent.h"
#include "SB_CharacterController.generated.h"

class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;

UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	Walking,
	Surfing
};

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
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RightWaveTriggerAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LeftWaveTriggerAction;
	// variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SurfMovement", meta = (AllowPrivateAccess = "true"))
	float SurfAcceleration = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SurfMovement", meta = (AllowPrivateAccess = "true"))
	float SurfMaxSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SurfMovement", meta = (AllowPrivateAccess = "true"))
	float SurfFriction = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SurfMovement", meta = (AllowPrivateAccess = "true"))
	float SurfTurnSpeed = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SurfMovement", meta = (AllowPrivateAccess = "true"))
	float SurfBrake = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SurfMovement", meta = (AllowPrivateAccess = "true"))
	float SurfWaterPushCoeffZ = 0.7f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SurfMovement", meta = (AllowPrivateAccess = "true"))
	float SurfJumpPower = 10000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX", meta = (AllowPrivateAccess = "true"))	
	UNiagaraComponent* TrailEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FX", meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* SplashEffect;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FX", meta = (AllowPrivateAccess = "true"))
	float SplashVelocityChangeReq = 15.f;


	FVector CurrentVelocity = FVector::ZeroVector;

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

	void StopMove(const FInputActionValue& Value);

	void SurfJump(const FInputActionValue& Value);

	void StopSurfJumping();

	void SpawnSplashEffect();

	USB_CharacterMovementComponent* GetUSBCharacterMovementComponent();

	void RightWaveTrigger(const FInputActionValue& Value);

	void LeftWaveTrigger(const FInputActionValue& Value);

	void SetCustomMovementMode(ECustomMovementMode NewMode);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Landed(const FHitResult& Hit) override; // Appelé quand on atterrit sur une surface	

private:
	ECustomMovementMode CurrentMovementMode = ECustomMovementMode::Surfing;
	float WaterSurfaceZ = 100.0f; // Hauteur de l'eau
	bool bMoving = false;
	bool bSurfJumping = false;

	FVector PreviousVelocity;
	float TurnThreshold = 1.f; // Seuil pour déclencher les éclaboussures
};
