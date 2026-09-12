// Copyright Epic Games, Inc. All Rights Reserved.

#include "sword_ue5_8Character.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "sword_ue5_8MeleeCombatComponent.h"
#include "sword_ue5_8.h"

Asword_ue5_8Character::Asword_ue5_8Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;

	// Create the melee combat component that drives the Attack/Block Duel mechanic
	MeleeCombatComponent = CreateDefaultSubobject<Usword_ue5_8MeleeCombatComponent>(TEXT("Melee Combat Component"));
}

void Asword_ue5_8Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &Asword_ue5_8Character::DoJumpStart);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &Asword_ue5_8Character::DoJumpEnd);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &Asword_ue5_8Character::MoveInput);

		// Looking/Aiming
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &Asword_ue5_8Character::LookInput);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &Asword_ue5_8Character::LookInput);

		// Attack/Block (see ADR-0003: while held, the mouse above drives the blade instead of the camera)
		if (MeleeCombatComponent)
		{
			MeleeCombatComponent->SetupPlayerInputComponent(EnhancedInputComponent);
		}
	}
	else
	{
		UE_LOG(Logsword_ue5_8, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void Asword_ue5_8Character::MoveInput(const FInputActionValue& Value)
{
	// get the Vector2D move axis
	FVector2D MovementVector = Value.Get<FVector2D>();

	// pass the axis values to the move input
	DoMove(MovementVector.X, MovementVector.Y);
}

void Asword_ue5_8Character::LookInput(const FInputActionValue& Value)
{
	// get the Vector2D look axis
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// ADR-0003: while Attack or Block is held, this same mouse/pen motion steers the blade instead of the camera
	if (MeleeCombatComponent && MeleeCombatComponent->IsVerbHeld())
	{
		MeleeCombatComponent->AddSteeringInput(LookAxisVector);
		return;
	}

	// pass the axis values to the aim input
	DoAim(LookAxisVector.X, LookAxisVector.Y);
}

void Asword_ue5_8Character::DoAim(float Yaw, float Pitch)
{
	if (GetController())
	{
		// pass the rotation inputs
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void Asword_ue5_8Character::DoMove(float Right, float Forward)
{
	// A real fencer plants their feet the moment they commit to a cut or a guard -- movement is
	// locked for the whole Attack/Block hold, not just once a swing Commits. See CONTEXT.md/ADR-0003.
	if (MeleeCombatComponent && MeleeCombatComponent->IsVerbHeld())
	{
		return;
	}

	if (GetController())
	{
		// pass the move inputs
		AddMovementInput(GetActorRightVector(), Right);
		AddMovementInput(GetActorForwardVector(), Forward);
	}
}

void Asword_ue5_8Character::DoJumpStart()
{
	// pass Jump to the character
	Jump();
}

void Asword_ue5_8Character::DoJumpEnd()
{
	// pass StopJumping to the character
	StopJumping();
}
