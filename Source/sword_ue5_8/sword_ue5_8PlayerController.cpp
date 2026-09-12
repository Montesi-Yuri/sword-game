// Copyright Epic Games, Inc. All Rights Reserved.

#include "sword_ue5_8PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "sword_ue5_8CameraManager.h"
#include "sword_ue5_8HealthBarWidget.h"
#include "sword_ue5_8PlayerState.h"
#include "Blueprint/UserWidget.h"
#include "sword_ue5_8.h"
#include "Widgets/Input/SVirtualJoystick.h"

Asword_ue5_8PlayerController::Asword_ue5_8PlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = Asword_ue5_8CameraManager::StaticClass();
}

void Asword_ue5_8PlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (IsLocalPlayerController() && ShouldUseTouchControls())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);
		}
		else
		{

			UE_LOG(Logsword_ue5_8, Error, TEXT("Could not spawn mobile controls widget."));
		}
	}

	SetupHealthBar();
}

void Asword_ue5_8PlayerController::SetupHealthBar()
{
	// only spawn the HP bar for local player controllers
	if (!IsLocalPlayerController() || !HealthBarWidgetClass)
	{
		return;
	}

	Asword_ue5_8PlayerState* DuelPlayerState = GetPlayerState<Asword_ue5_8PlayerState>();
	if (!DuelPlayerState)
	{
		UE_LOG(Logsword_ue5_8, Error, TEXT("'%s' has no Asword_ue5_8PlayerState yet; cannot bind the HP bar."), *GetNameSafe(this));
		return;
	}

	HealthBarWidget = CreateWidget<Usword_ue5_8HealthBarWidget>(this, HealthBarWidgetClass);
	if (!HealthBarWidget)
	{
		UE_LOG(Logsword_ue5_8, Error, TEXT("Could not spawn HP bar widget."));
		return;
	}

	HealthBarWidget->AddToViewport();
	HealthBarWidget->BP_UpdateHealthBar(DuelPlayerState->GetHealth(), DuelPlayerState->GetMaxHealth());

	DuelPlayerState->OnHealthChanged.AddDynamic(HealthBarWidget, &Usword_ue5_8HealthBarWidget::BP_UpdateHealthBar);
	DuelPlayerState->OnDuelLost.AddDynamic(HealthBarWidget, &Usword_ue5_8HealthBarWidget::BP_DuelLost);
}

void Asword_ue5_8PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

bool Asword_ue5_8PlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}
