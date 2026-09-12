// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "sword_ue5_8PlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;
class Usword_ue5_8HealthBarWidget;

/**
 *  Simple first person Player Controller
 *  Manages the input mapping context.
 *  Overrides the Player Camera Manager class.
 */
UCLASS(abstract, config = "Game")
class SWORD_UE5_8_API Asword_ue5_8PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/** Constructor */
	Asword_ue5_8PlayerController();

protected:
	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category = "Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** HP bar widget class to spawn for the local player. Assign a Blueprint subclass of Usword_ue5_8HealthBarWidget. */
	UPROPERTY(EditAnywhere, Category = "Duel|UI")
	TSubclassOf<Usword_ue5_8HealthBarWidget> HealthBarWidgetClass;

	/** Pointer to the spawned HP bar widget */
	UPROPERTY()
	TObjectPtr<Usword_ue5_8HealthBarWidget> HealthBarWidget;

	/** Spawns the HP bar widget and binds it to this controller's PlayerState */
	void SetupHealthBar();

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;
};
