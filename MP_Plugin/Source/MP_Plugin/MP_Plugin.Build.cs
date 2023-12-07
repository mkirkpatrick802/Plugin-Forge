// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MP_Plugin : ModuleRules
{
	public MP_Plugin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "OnlineSubsystemSteam", "OnlineSubsystem" });
	}
}
