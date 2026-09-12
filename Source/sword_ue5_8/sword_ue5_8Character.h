// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "sword_ue5_8Character.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;
class Usword_ue5_8MeleeCombatComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A basic first person character
 */
UCLASS(abstract)
class Asword_ue5_8Character : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: full-body skeleton rendered with first-person FOV/near-clip handling; seen only by self. Shares the same skeletal mesh asset as GetMesh(), kept in sync via a Copy Pose AnimBP -- not a separate arms-only rig. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Drives the free-path Attack/Block Duel mechanic -- see sword_ue5_8MeleeCombatComponent.h */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	Usword_ue5_8MeleeCombatComponent* MeleeCombatComponent;

protected:
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category = "Input")
	class UInputAction* MouseLookAction;

public:
	Asword_ue5_8Character();

protected:
	/** Called from Input Actions for movement input */
	void MoveInput(const FInputActionValue& Value);

	/** Called from Input Actions for looking input */
	void LookInput(const FInputActionValue& Value);

	/** Handles aim inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoAim(float Yaw, float Pitch);

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles jump start inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpStart();

	/** Handles jump end inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpEnd();

protected:
	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

public:
	/** Returns the first person mesh **/
	USkeletalMeshComponent* GetFirstPersonMesh() const
	{
		return FirstPersonMesh;
	}

	/** Returns first person camera component **/
	UCameraComponent* GetFirstPersonCameraComponent() const
	{
		return FirstPersonCameraComponent;
	}

	/** Returns the melee combat component **/
	Usword_ue5_8MeleeCombatComponent* GetMeleeCombatComponent() const
	{
		return MeleeCombatComponent;
	}
};
