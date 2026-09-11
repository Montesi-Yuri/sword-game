---
name: new-gameplay-class
description: Scaffold a new UE C++ gameplay class (Actor, ActorComponent, PlayerController, GameMode, etc.) for the sword_ue5_8 project, following this project's actual conventions. Use when the user asks to create a new class, component, controller, or similar C++ gameplay type.
---

Create a new C++ header/source pair for this Unreal Engine 5.8 project, matching the conventions actually used in `Source/sword_ue5_8/`.

## Steps

1. Ask (if not already clear from the request): class name, base class (e.g. `AActor`, `UActorComponent`, `APlayerController`), and which folder it belongs in — the root module folder, or a `Variant_*` subfolder. Since this project is a melee combat game (see CLAUDE.md), new gameplay classes should generally go in a new dedicated subfolder (e.g. `Source/sword_ue5_8/Combat/`) rather than inside `Variant_Horror`/`Variant_Shooter`, which are leftover template scaffolding.

2. If the class goes in a new subfolder not already listed in `Source/sword_ue5_8/sword_ue5_8.Build.cs`, add it to `PublicIncludePaths` there — headers in an unlisted folder won't resolve.

3. Write the header (`.h`) with:
   - Epic-style copyright header comment (`// Copyright Epic Games, Inc. All Rights Reserved.`), matching existing files.
   - `#pragma once`, appropriate includes (`CoreMinimal.h` plus the base class header), then the `.generated.h` include last.
   - Correct UE prefix for the class (`A` for Actor-derived, `U` for UObject/Component-derived, `F` for structs, `I` for interfaces).
   - `UCLASS()` and `GENERATED_BODY()`.
   - The `SWORD_UE5_8_API` export macro on the class declaration.

4. Write the source (`.cpp`) with the same copyright header, including the matching header first.

5. Follow naming and structure of the nearest existing analogous class in the codebase (e.g. mirror `ShooterNPC.h`/`.cpp` for an AI-driven pawn, or `ShooterWeapon.h`/`.cpp` for an equippable item) rather than inventing a new pattern — read that file first.

6. After creating the files, remind the user to regenerate project files (Visual Studio project files) if their IDE doesn't pick up the new files automatically — usually via right-clicking the `.uproject` and choosing "Generate Visual Studio project files", or via the Editor's "Refresh Visual Studio Project" tools menu.
