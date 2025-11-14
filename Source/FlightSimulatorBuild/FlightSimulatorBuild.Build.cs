// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FlightSimulatorBuild : ModuleRules
{
	public FlightSimulatorBuild(ReadOnlyTargetRules Target) : base(Target)
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
			"FlightSimulatorBuild",
			"FlightSimulatorBuild/Variant_Platforming",
			"FlightSimulatorBuild/Variant_Platforming/Animation",
			"FlightSimulatorBuild/Variant_Combat",
			"FlightSimulatorBuild/Variant_Combat/AI",
			"FlightSimulatorBuild/Variant_Combat/Animation",
			"FlightSimulatorBuild/Variant_Combat/Gameplay",
			"FlightSimulatorBuild/Variant_Combat/Interfaces",
			"FlightSimulatorBuild/Variant_Combat/UI",
			"FlightSimulatorBuild/Variant_SideScrolling",
			"FlightSimulatorBuild/Variant_SideScrolling/AI",
			"FlightSimulatorBuild/Variant_SideScrolling/Gameplay",
			"FlightSimulatorBuild/Variant_SideScrolling/Interfaces",
			"FlightSimulatorBuild/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
