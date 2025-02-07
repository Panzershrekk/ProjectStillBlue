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
    //ControlInputVector = Velocity;    
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
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        /*AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);*/
    }
}

void ASB_CharacterController::AddAccelerationInput(FVector WorldDirection, float ScaleValue) {
    Velocity += WorldDirection * ScaleValue * SurfAcceleration;
}

void ASB_CharacterController::Move(const FInputActionValue& Value)
{
    // input is a Vector2D
    FVector2D MovementVector = Value.Get<FVector2D>();

    /*if (Controller != nullptr)
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        //AddAccelerationInput(ForwardDirection, MovementVector.Y);
        //AddAccelerationInput(RightDirection, MovementVector.X);

        AddMovementInput(ForwardDirection, 1);
        //AddMovementInput(RightDirection, MovementVector.X);

        // (0,1,0) (Y,X,0)
        if (!MovementVector.IsNearlyZero()) {
            // Vecteur en 3D
            FVector InputDirection = FVector(MovementVector.X, MovementVector.Y, 0.0f);
            InputDirection.Normalize();

            // Vecteur vers le haut
            FVector UpVector = FVector(0.0f, 1.0f, 0.0f);

            // Angle entre les deux
            float AngleDegrees = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(UpVector, InputDirection)));

            // Angle positif ou negatif
            if (InputDirection.X < 0) {
                AngleDegrees = -AngleDegrees;
            }
            
            //ProcessRotation(FRotator(0.0f, AngleDegrees, 0.0f));
        }

    }*/
    if (Controller != nullptr && !MovementVector.IsNearlyZero())
    {
        float TargetYawOffset = MovementVector.X * MaxTurnAngle;
        float TargetYaw = GetVelocity().Rotation().Yaw + TargetYawOffset;

        float CurrentYaw = GetActorRotation().Yaw;
        float ClampedYaw = FMath::Clamp(TargetYaw, CurrentYaw - MaxTurnAngle, CurrentYaw + MaxTurnAngle);

        FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), FRotator(0.f, ClampedYaw, 0.f), GetWorld()->GetDeltaSeconds(), SurfRotationSpeed);
        SetActorRotation(NewRotation);

        AddMovementInput(GetActorForwardVector(), 1.0f);
    }

}

USB_CharacterMovementComponent* ASB_CharacterController::GetUSBCharacterMovementComponent() {
    return FindComponentByClass<USB_CharacterMovementComponent>();
}

void ASB_CharacterController::ProcessRotation(FRotator Rotation) {
    USB_CharacterMovementComponent* SurfCharacterMovement = GetUSBCharacterMovementComponent();
    SurfCharacterMovement->AddRotation(Rotation);
}



