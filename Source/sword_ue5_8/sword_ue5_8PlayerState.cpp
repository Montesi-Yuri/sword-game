// Copyright Epic Games, Inc. All Rights Reserved.

#include "sword_ue5_8PlayerState.h"

Asword_ue5_8PlayerState::Asword_ue5_8PlayerState()
{
	// Set here (not just BeginPlay) so GetHealth() is correct even if read before this actor's BeginPlay runs.
	CurrentHealth = MaxHealth;
}

void Asword_ue5_8PlayerState::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void Asword_ue5_8PlayerState::ApplyDamage(float DamageAmount)
{
	if (bIsDefeated || DamageAmount <= 0.0f)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.0f)
	{
		bIsDefeated = true;
		OnDuelLost.Broadcast();
	}
}

float Asword_ue5_8PlayerState::GetHealthPercent() const
{
	return MaxHealth > 0.0f ? CurrentHealth / MaxHealth : 0.0f;
}
