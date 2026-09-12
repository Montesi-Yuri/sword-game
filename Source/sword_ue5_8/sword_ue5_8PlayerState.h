// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "sword_ue5_8PlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDuelLost);

/**
 *  Tracks a Combatant's Health for the duration of a Duel.
 *  See CONTEXT.md for the Duel/Combatant/Health glossary.
 */
UCLASS()
class SWORD_UE5_8_API Asword_ue5_8PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	Asword_ue5_8PlayerState();

	/** Broadcast whenever Health changes, for UI to bind to */
	UPROPERTY(BlueprintAssignable, Category = "Duel")
	FOnHealthChanged OnHealthChanged;

	/** Broadcast once, the moment Health first reaches zero */
	UPROPERTY(BlueprintAssignable, Category = "Duel")
	FOnDuelLost OnDuelLost;

	/** Reduces Health by DamageAmount, clamped to zero. Has no effect once already defeated. */
	UFUNCTION(BlueprintCallable, Category = "Duel")
	void ApplyDamage(float DamageAmount);

	UFUNCTION(BlueprintPure, Category = "Duel")
	float GetHealth() const
	{
		return CurrentHealth;
	}

	UFUNCTION(BlueprintPure, Category = "Duel")
	float GetMaxHealth() const
	{
		return MaxHealth;
	}

	UFUNCTION(BlueprintPure, Category = "Duel")
	float GetHealthPercent() const;

	/** True once this Combatant's Health has reached zero */
	UFUNCTION(BlueprintPure, Category = "Duel")
	bool IsDefeated() const
	{
		return bIsDefeated;
	}

protected:
	virtual void BeginPlay() override;

	/** Health this Combatant starts a Duel with */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Duel")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Duel")
	float CurrentHealth = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Duel")
	bool bIsDefeated = false;
};
