// Copyright Epic Games, Inc. All Rights Reserved.

#include "sword_ue5_8MeleeCombatComponent.h"
#include "sword_ue5_8PlayerState.h"
#include "sword_ue5_8.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

Usword_ue5_8MeleeCombatComponent::Usword_ue5_8MeleeCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void Usword_ue5_8MeleeCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentBladeTipLocation = ComputeTargetWorldLocation(0.0f, 0.0f);
	PreviousBladeTipLocation = CurrentBladeTipLocation;
}

void Usword_ue5_8MeleeCombatComponent::SetupPlayerInputComponent(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (!EnhancedInputComponent)
	{
		return;
	}

	if (AttackAction)
	{
		// Also bound to Triggered (idempotent -- BeginAttack no-ops unless CurrentState == Guard) so
		// a still-held Attack button picks up the instant Block/recovery frees Guard back up, rather
		// than requiring a fresh press.
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &Usword_ue5_8MeleeCombatComponent::BeginAttack);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &Usword_ue5_8MeleeCombatComponent::BeginAttack);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Completed, this, &Usword_ue5_8MeleeCombatComponent::EndAttack);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Canceled, this, &Usword_ue5_8MeleeCombatComponent::EndAttack);
	}
	else
	{
		UE_LOG(Logsword_ue5_8, Warning, TEXT("'%s' has no AttackAction assigned; Attack is disabled."), *GetNameSafe(GetOwner()));
	}

	if (BlockAction)
	{
		// See the AttackAction comment above -- same self-healing rebind for a held Block button.
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, this, &Usword_ue5_8MeleeCombatComponent::BeginBlock);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Triggered, this, &Usword_ue5_8MeleeCombatComponent::BeginBlock);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &Usword_ue5_8MeleeCombatComponent::EndBlock);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Canceled, this, &Usword_ue5_8MeleeCombatComponent::EndBlock);
	}
	else
	{
		UE_LOG(Logsword_ue5_8, Warning, TEXT("'%s' has no BlockAction assigned; Block is disabled."), *GetNameSafe(GetOwner()));
	}
}

void Usword_ue5_8MeleeCombatComponent::BeginAttack()
{
	// Attack and Block are mutually exclusive -- ignore if already committed to the other verb.
	if (CurrentState != EMeleeCombatState::Guard)
	{
		return;
	}

	bAttackHeld = true;
	bResolvedThisSwing = false;
	CurrentState = EMeleeCombatState::WindingUp;
	TargetAzimuthDegrees = 0.0f;
	TargetElevationDegrees = 0.0f;
	PreviousTargetAzimuthDegrees = 0.0f;
	PreviousTargetElevationDegrees = 0.0f;
}

void Usword_ue5_8MeleeCombatComponent::EndAttack()
{
	bAttackHeld = false;

	if (CurrentState == EMeleeCombatState::WindingUp)
	{
		// Never crossed the Commit threshold -- a free cancel back to Guard, no Exposure.
		CurrentState = EMeleeCombatState::Guard;
	}
	else if (CurrentState == EMeleeCombatState::Committed && !bResolvedThisSwing)
	{
		// Released mid-swing without landing a hit or being Blocked -- a miss.
		EnterRecovering();
	}
}

void Usword_ue5_8MeleeCombatComponent::BeginBlock()
{
	if (CurrentState != EMeleeCombatState::Guard)
	{
		return;
	}

	bBlockHeld = true;
	CurrentState = EMeleeCombatState::Blocking;
	TargetAzimuthDegrees = 0.0f;
	TargetElevationDegrees = 0.0f;
	PreviousTargetAzimuthDegrees = 0.0f;
	PreviousTargetElevationDegrees = 0.0f;
}

void Usword_ue5_8MeleeCombatComponent::EndBlock()
{
	bBlockHeld = false;

	if (CurrentState == EMeleeCombatState::Blocking)
	{
		EnterRecovering();
	}
}

void Usword_ue5_8MeleeCombatComponent::EnterRecovering()
{
	CurrentState = EMeleeCombatState::Recovering;
	// The Target Point snaps to Guard; TickBladeSimulation eases the blade tip back to it over time.
	TargetAzimuthDegrees = 0.0f;
	TargetElevationDegrees = 0.0f;
}

void Usword_ue5_8MeleeCombatComponent::AddSteeringInput(FVector2D ScreenDelta)
{
	if (CurrentState != EMeleeCombatState::WindingUp && CurrentState != EMeleeCombatState::Committed && CurrentState != EMeleeCombatState::Blocking)
	{
		return;
	}

	TargetAzimuthDegrees = FMath::Clamp(TargetAzimuthDegrees + ScreenDelta.X * SteeringDegreesPerInputUnit, -MaxAzimuthDegrees, MaxAzimuthDegrees);
	TargetElevationDegrees = FMath::Clamp(TargetElevationDegrees - ScreenDelta.Y * SteeringDegreesPerInputUnit, -MaxElevationDegrees, MaxElevationDegrees);
}

FVector Usword_ue5_8MeleeCombatComponent::ComputeTargetWorldLocation(float AzimuthOffsetDegrees, float ElevationOffsetDegrees) const
{
	const AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return FVector::ZeroVector;
	}

	const FRotator OwnerRotation = OwnerActor->GetActorRotation();
	const FRotator SwingOffset(GuardElevationDegrees + ElevationOffsetDegrees, GuardAzimuthDegrees + AzimuthOffsetDegrees, 0.0f);
	const FQuat WorldSwingRotation = OwnerRotation.Quaternion() * SwingOffset.Quaternion();

	return GetShoulderAnchorLocation() + WorldSwingRotation.Vector() * ReachRadius;
}

FVector Usword_ue5_8MeleeCombatComponent::GetShoulderAnchorLocation() const
{
	if (const ACharacter* Character = Cast<ACharacter>(GetOwner()))
	{
		if (USkeletalMeshComponent* Mesh = Character->GetMesh())
		{
			return Mesh->GetSocketLocation(ShoulderSocketName);
		}
	}

	return GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;
}

Asword_ue5_8PlayerState* Usword_ue5_8MeleeCombatComponent::GetOpponentPlayerState() const
{
	if (const APawn* OpponentPawn = Cast<APawn>(Opponent))
	{
		return OpponentPawn->GetPlayerState<Asword_ue5_8PlayerState>();
	}

	return nullptr;
}

Usword_ue5_8MeleeCombatComponent* Usword_ue5_8MeleeCombatComponent::GetOpponentCombatComponent() const
{
	return Opponent ? Opponent->FindComponentByClass<Usword_ue5_8MeleeCombatComponent>() : nullptr;
}

void Usword_ue5_8MeleeCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (DeltaTime <= 0.0f)
	{
		return;
	}

	TickTargetPoint(DeltaTime);
	TickBladeSimulation(DeltaTime);

	if (CurrentState == EMeleeCombatState::WindingUp)
	{
		TickCommitCheck();
	}
	else if (CurrentState == EMeleeCombatState::Committed)
	{
		TickHitDetection();
	}
	else if (CurrentState == EMeleeCombatState::Recovering)
	{
		TickRecoveryCheck();
	}

	TickSoftFacingAssist(DeltaTime);

	if (bDrawDebug)
	{
		DrawDebug();
	}
}

void Usword_ue5_8MeleeCombatComponent::TickTargetPoint(float DeltaTime)
{
	const float DeltaAzimuth = TargetAzimuthDegrees - PreviousTargetAzimuthDegrees;
	const float DeltaElevation = TargetElevationDegrees - PreviousTargetElevationDegrees;
	CurrentTracedSpeedDegPerSec = FMath::Sqrt(DeltaAzimuth * DeltaAzimuth + DeltaElevation * DeltaElevation) / DeltaTime;

	PreviousTargetAzimuthDegrees = TargetAzimuthDegrees;
	PreviousTargetElevationDegrees = TargetElevationDegrees;
}

void Usword_ue5_8MeleeCombatComponent::TickBladeSimulation(float DeltaTime)
{
	// Critically-damped-ish spring chasing the Target Point -- see the class comment: this stands
	// in for ADR-0001's Chaos joint drives until a Physics Asset exists to drive for real.
	const FVector TargetLocation = ComputeTargetWorldLocation(TargetAzimuthDegrees, TargetElevationDegrees);
	const FVector ToTarget = TargetLocation - CurrentBladeTipLocation;
	const FVector Acceleration = ToTarget * SpringStiffness - CurrentBladeTipVelocity * SpringDamping;

	CurrentBladeTipVelocity += Acceleration * DeltaTime;
	PreviousBladeTipLocation = CurrentBladeTipLocation;
	CurrentBladeTipLocation += CurrentBladeTipVelocity * DeltaTime;
}

void Usword_ue5_8MeleeCombatComponent::TickCommitCheck()
{
	if (CurrentTracedSpeedDegPerSec >= CommitAngularSpeedThreshold)
	{
		CurrentState = EMeleeCombatState::Committed;
		CommittedSwingSpeedDegPerSec = CurrentTracedSpeedDegPerSec;
	}
}

void Usword_ue5_8MeleeCombatComponent::TickHitDetection()
{
	if (bResolvedThisSwing || !Opponent)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(MeleeSwing), false, GetOwner());

	FHitResult Hit;
	const bool bHit = World->SweepSingleByChannel(
		Hit,
		PreviousBladeTipLocation,
		CurrentBladeTipLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(BladeSweepRadius),
		QueryParams);

	if (!bHit || Hit.GetActor() != Opponent)
	{
		return;
	}

	const Usword_ue5_8MeleeCombatComponent* OpponentCombat = GetOpponentCombatComponent();
	const bool bBlocked = OpponentCombat && OpponentCombat->GetCombatState() == EMeleeCombatState::Blocking && FVector::Dist(OpponentCombat->GetBladeTipLocation(), Hit.Location) <= BlockInterceptRadius;

	bResolvedThisSwing = true;

	if (!bBlocked)
	{
		const float SpeedOverThreshold = FMath::Max(0.0f, CommittedSwingSpeedDegPerSec - CommitAngularSpeedThreshold);
		const float Damage = BaseDamage + SpeedOverThreshold * SpeedDamageMultiplier;

		if (Asword_ue5_8PlayerState* OpponentState = GetOpponentPlayerState())
		{
			OpponentState->ApplyDamage(Damage);
		}
	}

	EnterRecovering();
}

void Usword_ue5_8MeleeCombatComponent::TickRecoveryCheck()
{
	const FVector GuardLocation = ComputeTargetWorldLocation(0.0f, 0.0f);
	const bool bSettledInPlace = FVector::Dist(CurrentBladeTipLocation, GuardLocation) <= RecoveredDistanceTolerance;
	const bool bSettledInSpeed = CurrentBladeTipVelocity.Size() <= RecoveredSpeedTolerance;

	if (bSettledInPlace && bSettledInSpeed)
	{
		CurrentState = EMeleeCombatState::Guard;
	}
}

void Usword_ue5_8MeleeCombatComponent::TickSoftFacingAssist(float DeltaTime) const
{
	if (!IsVerbHeld() || !Opponent)
	{
		return;
	}

	// Go through the Controller's ControlRotation, not AActor::SetActorRotation: the camera reads
	// ControlRotation (bUsePawnControlRotation), and if the Pawn also has bUseControllerRotationYaw
	// set, PlayerController::UpdateRotation re-applies ControlRotation to the actor every tick anyway
	// -- a direct SetActorRotation would either be invisible to the camera or get overwritten right back.
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	AController* OwnerController = OwnerPawn ? OwnerPawn->GetController() : nullptr;
	if (!OwnerController)
	{
		return;
	}

	const FVector ToOpponent = Opponent->GetActorLocation() - GetOwner()->GetActorLocation();
	if (ToOpponent.IsNearlyZero())
	{
		return;
	}

	const float DesiredYaw = ToOpponent.Rotation().Yaw;
	const float CurrentYaw = OwnerController->GetControlRotation().Yaw;
	const float NewYaw = FMath::RInterpConstantTo(FRotator(0.0f, CurrentYaw, 0.0f), FRotator(0.0f, DesiredYaw, 0.0f), DeltaTime, SoftFacingMaxDegreesPerSecond).Yaw;

	FRotator NewControlRotation = OwnerController->GetControlRotation();
	NewControlRotation.Yaw = NewYaw;
	OwnerController->SetControlRotation(NewControlRotation);
}

void Usword_ue5_8MeleeCombatComponent::DrawDebug() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector Shoulder = GetShoulderAnchorLocation();
	FColor StateColor = FColor::White;
	switch (CurrentState)
	{
	case EMeleeCombatState::Guard:
		StateColor = FColor::White;
		break;
	case EMeleeCombatState::WindingUp:
		StateColor = FColor::Yellow;
		break;
	case EMeleeCombatState::Committed:
		StateColor = FColor::Red;
		break;
	case EMeleeCombatState::Recovering:
		StateColor = FColor::Orange;
		break;
	case EMeleeCombatState::Blocking:
		StateColor = FColor::Cyan;
		break;
	}

	DrawDebugLine(World, Shoulder, CurrentBladeTipLocation, StateColor, false, -1.0f, 0, 1.5f);
	DrawDebugSphere(World, CurrentBladeTipLocation, BladeSweepRadius, 8, StateColor, false, -1.0f, 0, 1.0f);
}
