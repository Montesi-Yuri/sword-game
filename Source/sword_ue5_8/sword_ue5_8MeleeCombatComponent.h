// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "sword_ue5_8MeleeCombatComponent.generated.h"

class UEnhancedInputComponent;
class UInputAction;
class Asword_ue5_8PlayerState;

/**
 *  See CONTEXT.md for the Duel/Combatant/Attack/Block/Target Point/Reach Sphere/
 *  Commit/Wind-up/Recovery glossary these states and fields implement.
 */
UENUM(BlueprintType)
enum class EMeleeCombatState : uint8
{
	Guard,
	WindingUp,
	Committed,
	Recovering,
	Blocking
};

/**
 *  Drives the free-path Attack/Block mechanic described in ADR-0001, ADR-0003 and ADR-0004:
 *  a live-steered Target Point on a shoulder-anchored Reach Sphere, chased by a spring/damper
 *  ("PD controller") simulation of the blade tip, with trace-based hit and block detection.
 *
 *  The spring/damper here is a lightweight numerical approximation of ADR-0001's "physics-driven"
 *  requirement -- it delivers the same weighty, lagging-behind-input feel without requiring a
 *  Chaos Physics Asset (constraint bodies for the arm) to be authored in the Editor first. Swapping
 *  in real Chaos joint drives later is a drop-in replacement for TickBladeSimulation alone; nothing
 *  else in this component depends on which one is used.
 *
 *  Both a mouse-driven player and a future AI opponent drive this identical component: the player
 *  calls AddSteeringInput from raw mouse/pen delta, and an AI would call the same function fed by
 *  its own generated steering values -- see ADR-0004.
 *
 *  There is currently no real skeletal arm/blade to visually follow the simulated blade tip; until
 *  an AnimBP Two-Bone-IK node is wired up to consume GetShoulderAnchorLocation()/GetBladeTipLocation()
 *  (an Editor content task), bDrawDebug visualizes the mechanic directly.
 */
UCLASS(ClassGroup = (Duel), meta = (BlueprintSpawnableComponent))
class SWORD_UE5_8_API Usword_ue5_8MeleeCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	Usword_ue5_8MeleeCombatComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Call from the owning Pawn's SetupPlayerInputComponent to bind Attack/Block. */
	void SetupPlayerInputComponent(UEnhancedInputComponent* EnhancedInputComponent);

	/**
	 *  True while Attack or Block is physically held. The owning Character should route mouse-look
	 *  input to AddSteeringInput instead of the camera, and suppress movement input, while this is true.
	 */
	UFUNCTION(BlueprintPure, Category = "Duel")
	bool IsVerbHeld() const
	{
		return bAttackHeld || bBlockHeld;
	}

	/** Feeds live steering input (mouse/pen delta, or an AI-synthesized equivalent) driving the Target Point. */
	UFUNCTION(BlueprintCallable, Category = "Duel")
	void AddSteeringInput(FVector2D ScreenDelta);

	/** The other Combatant in this Duel. Wire this up for both sides before play (e.g. from the GameMode). */
	UFUNCTION(BlueprintCallable, Category = "Duel")
	void SetOpponent(AActor* InOpponent)
	{
		Opponent = InOpponent;
	}

	UFUNCTION(BlueprintPure, Category = "Duel")
	EMeleeCombatState GetCombatState() const
	{
		return CurrentState;
	}

	UFUNCTION(BlueprintPure, Category = "Duel")
	FVector GetShoulderAnchorLocation() const;

	UFUNCTION(BlueprintPure, Category = "Duel")
	FVector GetBladeTipLocation() const
	{
		return CurrentBladeTipLocation;
	}

protected:
	virtual void BeginPlay() override;

	void BeginAttack();
	void EndAttack();
	void BeginBlock();
	void EndBlock();

	void TickTargetPoint(float DeltaTime);
	void TickBladeSimulation(float DeltaTime);
	void TickCommitCheck();
	void TickHitDetection();
	void TickRecoveryCheck();
	void TickSoftFacingAssist(float DeltaTime) const;
	void EnterRecovering();
	void DrawDebug() const;

	FVector ComputeTargetWorldLocation(float AzimuthOffsetDegrees, float ElevationOffsetDegrees) const;
	Asword_ue5_8PlayerState* GetOpponentPlayerState() const;
	class Usword_ue5_8MeleeCombatComponent* GetOpponentCombatComponent() const;

	// --- Input ---

	/** Assign an IA_Attack Input Action (bool/digital) in the owning Character's defaults. */
	UPROPERTY(EditAnywhere, Category = "Duel|Input")
	TObjectPtr<UInputAction> AttackAction;

	/** Assign an IA_Block Input Action (bool/digital) in the owning Character's defaults. */
	UPROPERTY(EditAnywhere, Category = "Duel|Input")
	TObjectPtr<UInputAction> BlockAction;

	UPROPERTY(EditAnywhere, Category = "Duel|Input")
	float SteeringDegreesPerInputUnit = 0.6f;

	// --- Reach Sphere ---

	UPROPERTY(EditAnywhere, Category = "Duel|Reach Sphere")
	FName ShoulderSocketName = "clavicle_r";

	UPROPERTY(EditAnywhere, Category = "Duel|Reach Sphere")
	float ReachRadius = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Duel|Reach Sphere")
	float MaxAzimuthDegrees = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Duel|Reach Sphere")
	float MaxElevationDegrees = 70.0f;

	/** Guard direction offsets -- where the blade rests, and what the Target Point resets to on Recovery. */
	UPROPERTY(EditAnywhere, Category = "Duel|Reach Sphere")
	float GuardAzimuthDegrees = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Duel|Reach Sphere")
	float GuardElevationDegrees = -15.0f;

	// --- Attack ---

	/** Traced angular speed (deg/s) required to cross from Wind-up into Commit. */
	UPROPERTY(EditAnywhere, Category = "Duel|Attack")
	float CommitAngularSpeedThreshold = 180.0f;

	UPROPERTY(EditAnywhere, Category = "Duel|Attack")
	float BaseDamage = 15.0f;

	/** Extra damage per (deg/s) of traced speed above the commit threshold. */
	UPROPERTY(EditAnywhere, Category = "Duel|Attack")
	float SpeedDamageMultiplier = 0.15f;

	UPROPERTY(EditAnywhere, Category = "Duel|Attack")
	float BladeSweepRadius = 4.0f;

	// --- Blade simulation (spring/damper approximation of ADR-0001's physics-driven arm) ---

	UPROPERTY(EditAnywhere, Category = "Duel|Physics Approximation")
	float SpringStiffness = 40.0f;

	UPROPERTY(EditAnywhere, Category = "Duel|Physics Approximation")
	float SpringDamping = 9.0f;

	// --- Recovery ---

	UPROPERTY(EditAnywhere, Category = "Duel|Recovery")
	float RecoveredDistanceTolerance = 3.0f;

	UPROPERTY(EditAnywhere, Category = "Duel|Recovery")
	float RecoveredSpeedTolerance = 20.0f;

	// --- Block ---

	UPROPERTY(EditAnywhere, Category = "Duel|Block")
	float BlockInterceptRadius = 25.0f;

	// --- Camera ---

	/** Rate cap for the soft auto-facing assist while a verb is held (see ADR-0003). */
	UPROPERTY(EditAnywhere, Category = "Duel|Camera")
	float SoftFacingMaxDegreesPerSecond = 60.0f;

	UPROPERTY(EditAnywhere, Category = "Duel|Debug")
	bool bDrawDebug = true;

	UPROPERTY(EditInstanceOnly, Category = "Duel")
	TObjectPtr<AActor> Opponent;

	// --- Runtime state ---

	EMeleeCombatState CurrentState = EMeleeCombatState::Guard;

	bool bAttackHeld = false;
	bool bBlockHeld = false;
	bool bResolvedThisSwing = false;

	/** Accumulated steering offsets (degrees) from the Guard direction. */
	float TargetAzimuthDegrees = 0.0f;
	float TargetElevationDegrees = 0.0f;
	float PreviousTargetAzimuthDegrees = 0.0f;
	float PreviousTargetElevationDegrees = 0.0f;

	/** How fast the Target Point is currently being traced (deg/s) -- drives Commit and damage. */
	float CurrentTracedSpeedDegPerSec = 0.0f;

	/** The speed the swing committed at, used for its damage-on-hit. */
	float CommittedSwingSpeedDegPerSec = 0.0f;

	FVector CurrentBladeTipLocation = FVector::ZeroVector;
	FVector PreviousBladeTipLocation = FVector::ZeroVector;
	FVector CurrentBladeTipVelocity = FVector::ZeroVector;
};
