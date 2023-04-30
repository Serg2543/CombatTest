// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CombatTest : ModuleRules
{
	public CombatTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule" });

        // https://stackoverflow.com/questions/72379796/how-to-change-include-default-file-path-for-sub-folders
        PublicIncludePaths.AddRange(
        new string[] {
            "CombatTest"
        });
    }
}
