// Fill out your copyright notice in the Description page of Project Settings.

#include "SB_CharacterController.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

// Sets default values
ASB_CharacterController::ASB_CharacterController()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate


	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	TrailEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailEffect"));
	TrailEffect->SetupAttachment(RootComponent);
	TrailEffect->bAutoActivate = true;
}

// Called when the game starts or when spawned
void ASB_CharacterController::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ASB_CharacterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector CurrentPos = GetActorLocation();
	EMovementMode CurrentMode = GetCharacterMovement()->MovementMode;

	// Simulation de flotaison (ultra basique)
	if (CurrentMovementMode == ECustomMovementMode::Surfing)
	{
		// Si on est en chute libre, on ne stabilise pas encore
		if (CurrentPos.Z < WaterSurfaceZ) // Seuil ajustable
		{
			if (bSurfJumping) {
				bSurfJumping = false;
			}
			GetCharacterMovement()->GravityScale = 0.0f;
			CurrentVelocity.Z += (WaterSurfaceZ - CurrentPos.Z) * SurfWaterPushCoeffZ;
			GetCharacterMovement()->Velocity = CurrentVelocity;
		}
		else {
			GetCharacterMovement()->GravityScale = 1.0f;
		}
	}
	else if (CurrentMode == MOVE_Falling && CurrentMovementMode == ECustomMovementMode::Walking)
	{
		if (CurrentPos.Z < WaterSurfaceZ) {
			// Switch comportement eau
			SetCustomMovementMode(ECustomMovementMode::Surfing);
			CurrentVelocity = GetCharacterMovement()->Velocity;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("bMoving: %i"), bMoving);
	if (!bMoving) {
		
		CurrentVelocity = GetCharacterMovement()->Velocity*SurfBrake;
		GetCharacterMovement()->Velocity = CurrentVelocity;
	}	
	
	// DEBUG LOGS
	UE_LOG(LogTemp, Warning, TEXT("CurrentVelocity: %s | Speed: %f | MaxSpeed : %f"), *CurrentVelocity.ToString(), CurrentVelocity.Size(), GetCharacterMovement()->GetMaxSpeed());
	UE_LOG(LogTemp, Warning, TEXT("SurfAcceleration: %f, SurfMaxSpeed: %f, SurfFriction: %f, SurfTurnSpeed: %f"),
		SurfAcceleration, SurfMaxSpeed, SurfFriction, SurfTurnSpeed);

}

// Called to bind functionality to input
void ASB_CharacterController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASB_CharacterController::Look);
		// MOVING                
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASB_CharacterController::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ASB_CharacterController::StopMove);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &ASB_CharacterController::StopMove);
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASB_CharacterController::SurfJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ASB_CharacterController::StopSurfJumping);

		// Waves
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASB_CharacterController::RightWaveTrigger);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASB_CharacterController::LeftWaveTrigger);
	}
}

void ASB_CharacterController::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ASB_CharacterController::RightWaveTrigger(const FInputActionValue& Value) {

}


void ASB_CharacterController::LeftWaveTrigger(const FInputActionValue& Value) {

}

void ASB_CharacterController::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASB_CharacterController::StopMove(const FInputActionValue& Value) {
	UE_LOG(LogTemp, Warning, TEXT("stopmove"));
	bMoving = false;
}

void ASB_CharacterController::SurfJump(const FInputActionValue& Value) {
	if (CurrentMovementMode == ECustomMovementMode::Surfing && !bSurfJumping) {
		bSurfJumping = true;
		LaunchCharacter(FVector(0, 0, 100000 * GetWorld()->GetDeltaSeconds()), false, false);
	}
}

void ASB_CharacterController::StopSurfJumping() {

}

void ASB_CharacterController::Move(const FInputActionValue& Value)
{
	bMoving = true;
	FVector2D InputVector = Value.Get<FVector2D>();
	// Récupérer la direction de la caméra
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	FRotator CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	FVector ForwardVector = FRotationMatrix(CameraRotation).GetScaledAxis(EAxis::X);
	FVector RightVector = FRotationMatrix(CameraRotation).GetScaledAxis(EAxis::Y);
	ForwardVector.Normalize();
	RightVector.Normalize();
	// Calculer la direction de mouvement
	FVector MoveDirection = ForwardVector * InputVector.Y + RightVector * InputVector.X;
	MoveDirection.Z = 0;
	MoveDirection.Normalize();

	// Si on est en mode Surfing
	if (CurrentMovementMode == ECustomMovementMode::Surfing)
	{		
		// Appliquer la rotation progressivement
		FRotator TargetRotation = MoveDirection.Rotation();
		FRotator CurrentRotation = GetActorRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), SurfTurnSpeed);

		AddActorWorldRotation(FRotator(0.0f, NewRotation.Yaw - CurrentRotation.Yaw, 0.0f));

		// Ajouter la vitesse progressive pour glisser
		CurrentVelocity += MoveDirection * SurfAcceleration * GetWorld()->GetDeltaSeconds();
		CurrentVelocity = CurrentVelocity.GetClampedToMaxSize(SurfMaxSpeed);

		// Appliquer le mouvement
		if (!CurrentVelocity.IsNearlyZero())
		{
			AddMovementInput(CurrentVelocity.GetSafeNormal(), CurrentVelocity.Size() / SurfMaxSpeed);
		}		
	}
	else if (CurrentMovementMode == ECustomMovementMode::Walking)
	{
		// add movement 
		AddMovementInput(MoveDirection.GetSafeNormal());		

		// Appliquer la rotation progressivement
		FRotator TargetRotation = MoveDirection.Rotation();
		FRotator CurrentRotation = GetActorRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 100.f);

		AddActorWorldRotation(FRotator(0.0f, NewRotation.Yaw - CurrentRotation.Yaw, 0.0f));
	}	
}

void ASB_CharacterController::SetCustomMovementMode(ECustomMovementMode NewMode)
{
	CurrentMovementMode = NewMode;

	if (NewMode == ECustomMovementMode::Walking)
	{
		UE_LOG(LogTemp, Warning, TEXT("Je marche"));
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		GetCharacterMovement()->MaxWalkSpeed = 500.f;
		GetCharacterMovement()->BrakingDecelerationWalking = 2048.0f; // Ajuste la décélération
		GetCharacterMovement()->GroundFriction = 8.0f; // Ajuste la friction
		GetCharacterMovement()->GravityScale = 1.0f; // Gravité normale sur terre
		TrailEffect->Deactivate();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Je surf"));
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		GetCharacterMovement()->MaxWalkSpeed = SurfMaxSpeed;
		GetCharacterMovement()->BrakingDecelerationFalling = 0.0f; // Pas de freinage automatique
		GetCharacterMovement()->GroundFriction = 0.0f; // Glisse
		GetCharacterMovement()->GravityScale = 1.0f;
		TrailEffect->Activate();
	}
}

void ASB_CharacterController::Landed(const FHitResult& Hit)
{
	SetCustomMovementMode(ECustomMovementMode::Walking);
	Super::OnLanded(Hit); // Appelle la version de la méthode dans la classe parente (ACharacter)
	UE_LOG(LogTemp, Warning, TEXT("Le personnage a atterri"));
}

USB_CharacterMovementComponent* ASB_CharacterController::GetUSBCharacterMovementComponent() {
	return FindComponentByClass<USB_CharacterMovementComponent>();
}



