// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ZadanieTestoweCover : ModuleRules
{
	public ZadanieTestoweCover(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule", "Niagara" });
    }
}
