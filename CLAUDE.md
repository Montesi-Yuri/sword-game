# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project

Unreal Engine 5.8 C++ project. It started from Epic's stock First Person Template (redirects in `Config/DefaultEngine.ini` show the rename from `TP_FirstPerson` → `sword_ue5_8`) and is being built into a **melee combat ("sword") game**. The `Source/sword_ue5_8/Variant_Horror` and `Variant_Shooter` folders are leftover template scaffolding from the wizard, not the design direction — don't treat them as canonical patterns to extend for new gameplay.

## Module structure

- Single module `sword_ue5_8` under `Source/` — flat layout, no `Public/`/`Private` split. Each `Variant_*` subfolder is manually listed in `PublicIncludePaths` inside `sword_ue5_8.Build.cs`; a new subfolder must be added there or its headers won't resolve.
- Module dependencies: `Core, CoreUObject, Engine, InputCore, EnhancedInput, AIModule, StateTreeModule, GameplayStateTreeModule, UMG, Slate`.
- Plugins enabled: `ModelingToolsEditorMode` (editor-only), `StateTree`, `GameplayStateTree`.

## Build & iterate

- Recommended workflow: open `sword_ue5_8.sln` in Visual Studio and build the `sword_ue5_8Editor` target (Development Editor config). While the Editor is running, use Unreal's **Live Coding** for fast C++ iteration instead of rebuilding from VS each time.
- Two solutions exist at the root: `sword_ue5_8.sln` (the actual project) and `Automation_sword_ue5_8.sln` (UnrealBuildTool's standard engine-tooling solution, not custom test infra — ignore it).
- No automation tests or custom build/CI scripts exist in this project yet.

## Config gotchas

- `Config/DefaultGame.ini` still has `ProjectName=First Person Template`, a leftover from the template that was never updated.
- `Config/DefaultEngine.ini` has class/package redirects from `TP_FirstPerson` — don't remove these unless all content references have been migrated off the old names.
- The default startup map is `/Game/FirstPerson/Lvl_FirstPerson` with a **Blueprint** GameMode (`BP_FirstPersonGameMode`), not the C++ `Asword_ue5_8GameMode`.
- Rendering defaults are high-end: Lumen (GI + reflections), full ray tracing, Substrate, and virtual shadow maps are all enabled in `DefaultEngine.ini` — factor this in for any performance-related advice.

## Conventions

- Standard UE naming: `A`-prefix Actors, `U`-prefix UObjects/Components, `F`-prefix structs, `I`-prefix interfaces.
- Use the `SWORD_UE5_8_API` export macro on classes that need cross-module export.
- Existing files retain Epic's copyright header comment; keep it on new files for consistency.

## Source control

Not yet a git repo (git is planned). When initializing git, exclude `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/`, and `.vs/` — all currently untracked build/cache output sitting at the project root.
