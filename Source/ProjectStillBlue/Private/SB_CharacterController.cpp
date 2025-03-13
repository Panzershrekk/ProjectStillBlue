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
#include "SB_Wave.h"

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

	// VFX
	TrailEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailEffect"));
	TrailEffect->SetupAttachment(RootComponent);
	TrailEffect->bAutoActivate = true;

	SplashEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SplashEffect"));
	SplashEffect->SetupAttachment(RootComponent);
	SplashEffect->bAutoActivate = false;

}

// Called when the game starts or when spawned
void ASB_CharacterController::BeginPlay()
{
	Super::BeginPlay();

}

void ASB_CharacterController::BoostSpeed() 
{
	UE_LOG(LogTemp, Warning, TEXT("BoostSpeed !"));
	GetCharacterMovement()->MaxWalkSpeed *= 1.5f;
	SurfMaxSpeed *= 1.5f;
	GetWorldTimerManager().SetTimer(WaveTimerHandle, this, &ASB_CharacterController::ResetSpeed, 2.0f, false);
}

void ASB_CharacterController::ResetSpeed()
{
	GetCharacterMovement()->MaxWalkSpeed = 1200.f;
	SurfMaxSpeed = 1200.f;
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
			if (CurrentPos.Z < WaterSurfaceZ + 10.f) {
				if (fabsf(CurrentVelocity.Size() - PreviousVelocity.Size()) > SplashVelocityChangeReq) {
					SpawnSplashEffect();
				}
			}
		}

		PreviousVelocity = CurrentVelocity;
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

void ASB_CharacterController::SpawnSplashEffect()
{
	if (SplashEffect)
	{
		SplashEffect->Activate(true);
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
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ASB_CharacterController::StopMove);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &ASB_CharacterController::StopMove);
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ASB_CharacterController::SurfJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ASB_CharacterController::StopSurfJumping);

		// Waves
		EnhancedInputComponent->BindAction(RightWaveTriggerAction, ETriggerEvent::Triggered, this, &ASB_CharacterController::RightWaveTrigger);
		EnhancedInputComponent->BindAction(LeftWaveTriggerAction, ETriggerEvent::Triggered, this, &ASB_CharacterController::LeftWaveTrigger);
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
	this->SpawnWave(false);
}


void ASB_CharacterController::LeftWaveTrigger(const FInputActionValue& Value) {
	this->SpawnWave(true);
}

void ASB_CharacterController::ResetWaveCooldown() {
	UE_LOG(LogTemp, Warning, TEXT("ResetWaveCooldown() appel� !"));
	bCanSpawnWave = true;
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
		LaunchCharacter(FVector(0, 0, SurfJumpPower * GetWorld()->GetDeltaSeconds()), false, false);
	}
}

void ASB_CharacterController::StopSurfJumping() {

}

void ASB_CharacterController::Move(const FInputActionValue& Value)
{
	bMoving = true;
	FVector2D InputVector = Value.Get<FVector2D>();
	// R�cup�rer la direction de la cam�ra
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
		GetCharacterMovement()->BrakingDecelerationWalking = 2048.0f; // Ajuste la d�c�l�ration
		GetCharacterMovement()->GroundFriction = 8.0f; // Ajuste la friction
		GetCharacterMovement()->GravityScale = 1.0f; // Gravit� normale sur terre
		TrailEffect->Deactivate();
		GetWorldTimerManager().ClearTimer(WaveTimerHandle);
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

void ASB_CharacterController::SpawnWave(bool bIsLeft)
{
	if (WaveClass && CurrentMovementMode == ECustomMovementMode::Surfing)
	{
		// V�rifie si le joueur peut spawn une vague
		if (!bCanSpawnWave)
		{
			return;
		}

		// V�rifie si le joueur respecte l�alternance
		if ((bIsLeft && bLastWaveWasLeft) || (!bIsLeft && !bLastWaveWasLeft))
		{
			return; // Emp�che de spawn deux fois du m�me c�t�
		}

		// Bloque la cr�ation de nouvelles vagues jusqu'� la fin du cooldown
		bCanSpawnWave = false;
		bLastWaveWasLeft = bIsLeft;

		FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 1500.f;
		SpawnLocation.Z = WaterSurfaceZ-180.f;
		SpawnLocation += bIsLeft ? -GetActorRightVector() * 280.f : GetActorRightVector() * 280.f;

		FRotator SpawnRotation = GetActorRotation();
		if (bIsLeft) {
			SpawnRotation.Yaw += 90.0f;
		}
		else {
			SpawnRotation.Yaw -= 80.0f;
		}
		GetWorld()->SpawnActor<ASB_Wave>(WaveClass, SpawnLocation, SpawnRotation);

		// D�marre le cooldown
		GetWorldTimerManager().SetTimer(WaveCooldownTimerHandle, this, &ASB_CharacterController::ResetWaveCooldown, 1.5f, false);
	}
}

void ASB_CharacterController::Landed(const FHitResult& Hit)
{
	ASB_Wave* Wave = Cast<ASB_Wave>(Hit.GetActor());

	if (!Wave) {
		SetCustomMovementMode(ECustomMovementMode::Walking);
		Super::OnLanded(Hit); // Appelle la version de la m�thode dans la classe parente (ACharacter)
		//UE_LOG(LogTemp, Warning, TEXT("Le personnage a atterri"));
	}
}

USB_CharacterMovementComponent* ASB_CharacterController::GetUSBCharacterMovementComponent() {
	return FindComponentByClass<USB_CharacterMovementComponent>();
}



