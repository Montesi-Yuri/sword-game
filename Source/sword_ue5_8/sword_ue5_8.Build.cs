// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class sword_ue5_8 : ModuleRules
{
	public sword_ue5_8(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"sword_ue5_8",
			"sword_ue5_8/Variant_Horror",
			"sword_ue5_8/Variant_Horror/UI",
			"sword_ue5_8/Variant_Shooter",
			"sword_ue5_8/Variant_Shooter/AI",
			"sword_ue5_8/Variant_Shooter/UI",
			"sword_ue5_8/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
