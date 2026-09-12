// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "sword_ue5_8HealthBarWidget.generated.h"

/**
 *  Simple HP bar UI widget for a Duel. Subclass this in Blueprint to build the
 *  actual visual layout; the C++ base only exposes the events to bind to.
 */
UCLASS(abstract)
class SWORD_UE5_8_API Usword_ue5_8HealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Allows Blueprint to update the bar with the new Health values */
	UFUNCTION(BlueprintImplementableEvent, Category = "Duel", meta = (DisplayName = "UpdateHealthBar"))
	void BP_UpdateHealthBar(float CurrentHealth, float MaxHealth);

	/** Allows Blueprint to react to the Duel being lost (e.g. play a defeat screen) */
	UFUNCTION(BlueprintImplementableEvent, Category = "Duel", meta = (DisplayName = "DuelLost"))
	void BP_DuelLost();
};
