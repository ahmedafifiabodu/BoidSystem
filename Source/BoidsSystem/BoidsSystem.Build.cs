// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BoidsSystem : ModuleRules
{
	public BoidsSystem(ReadOnlyTargetRules Target) : base(Target)
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
			"BoidsSystem",
			"BoidsSystem/Variant_Platforming",
			"BoidsSystem/Variant_Platforming/Animation",
			"BoidsSystem/Variant_Combat",
			"BoidsSystem/Variant_Combat/AI",
			"BoidsSystem/Variant_Combat/Animation",
			"BoidsSystem/Variant_Combat/Gameplay",
			"BoidsSystem/Variant_Combat/Interfaces",
			"BoidsSystem/Variant_Combat/UI",
			"BoidsSystem/Variant_SideScrolling",
			"BoidsSystem/Variant_SideScrolling/AI",
			"BoidsSystem/Variant_SideScrolling/Gameplay",
			"BoidsSystem/Variant_SideScrolling/Interfaces",
			"BoidsSystem/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
