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

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

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
	float WaterHeight = 0.0f; // Altitude de la surface de l'eau
	FVector CurrentPos = GetActorLocation();
	EMovementMode CurrentMode = GetCharacterMovement()->MovementMode;	

	// Simulation de flotaison (ultra basique)
	if (CurrentMovementMode == ECustomMovementMode::Surfing)
	{
		// Vérifie la hauteur de l’eau et ajuste la position du personnage
		if (CurrentPos.Z < WaterHeight)
		{
			float FloatForce = 10.0f; // Force qui pousse vers le haut
			FVector UpForce = FVector(0, 0, FloatForce * (WaterHeight - CurrentPos.Z*1.5f) * DeltaTime);
			GetCharacterMovement()->Velocity += UpForce;
		}
	}
	else if (CurrentMode == MOVE_Falling)
	{
		if (CurrentPos.Z < WaterHeight) {
			// Switch comportement eau
			SetCustomMovementMode(ECustomMovementMode::Surfing);
		}
	}
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

void ASB_CharacterController::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D InputVector = Value.Get<FVector2D>();
	// Récupère la direction du joystick
	FVector InputDirection = FVector(InputVector.X, InputVector.Y, 0.0f);

	// Récupère la caméra
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	// Récupère la direction de la caméra, projetée sur le plan horizontal
	FRotator CameraRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
	FVector ForwardVector = FRotationMatrix(CameraRotation).GetScaledAxis(EAxis::X); // Avant de la caméra
	FVector RightVector = FRotationMatrix(CameraRotation).GetScaledAxis(EAxis::Y);   // Droite de la caméra

	// Calcul de la direction du mouvement basé sur l'input du joystick
	FVector MoveDirection = ForwardVector * InputVector.Y + RightVector * InputVector.X;

	// Normaliser la direction du mouvement pour éviter que le personnage ne se déplace plus vite en diagonale
	MoveDirection.Normalize();

	if (CurrentMovementMode == ECustomMovementMode::Surfing)
	{
		if (!MoveDirection.IsNearlyZero())
		{
			MoveDirection.Normalize();

			// Rotation progressive
			FRotator TargetRotation = MoveDirection.Rotation();
			FRotator CurrentRotation = GetActorRotation();
			FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), SurfTurnSpeed); // Ajuste la valeur ici pour lisser le virage

			AddActorWorldRotation(FRotator(0.0f, NewRotation.Yaw - CurrentRotation.Yaw, 0.0f));

			// Ajoute progressivement de la vitesse
			CurrentVelocity += MoveDirection * SurfAcceleration * GetWorld()->GetDeltaSeconds();
			CurrentVelocity = CurrentVelocity.GetClampedToMaxSize(SurfMaxSpeed);
		}
		else
		{
			// Réduction progressive de la vitesse pour simuler l’inertie
			float Speed = CurrentVelocity.Size();
			Speed = FMath::Max(Speed - SurfFriction * GetWorld()->GetDeltaSeconds(), 0.0f);
			CurrentVelocity = CurrentVelocity.GetSafeNormal() * Speed;
		}

		// Applique le mouvement
		if (!CurrentVelocity.IsNearlyZero())
		{
			AddMovementInput(CurrentVelocity.GetSafeNormal(), CurrentVelocity.Size() / SurfMaxSpeed);
		}
	}
	else
	{
		// Mode marche normal
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, InputVector.Y);
		AddMovementInput(RightDirection, InputVector.X);

		// Rotation
		FRotator TargetRotation = MoveDirection.Rotation();
		FRotator CurrentRotation = GetActorRotation();
		FRotator NewRotation = TargetRotation;//FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), SurfTurnSpeed); // Ajuste la valeur ici pour lisser le virage

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
		GetCharacterMovement()->BrakingDecelerationWalking = 2048.0f; // Ajuste la décélération
		GetCharacterMovement()->GroundFriction = 8.0f; // Ajuste la friction
		GetCharacterMovement()->GravityScale = 1.0f; // Gravité normale sur terre
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Je surf"));
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		GetCharacterMovement()->BrakingDecelerationFlying = 0.0f; // Pas de freinage automatique
		GetCharacterMovement()->GroundFriction = 0.0f; // Glisse
		GetCharacterMovement()->GravityScale = 0.5f; // Gravité réduite pour flotter légèrement
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



