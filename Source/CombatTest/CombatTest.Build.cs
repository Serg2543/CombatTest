// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CombatTest : ModuleRules
{
	public CombatTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        // Some compile time suggestions tests
        //MinFilesUsingPrecompiledHeaderOverride = 1;
        bEnforceIWYU = true;
        bUseUnity = true; // false

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "NavigationSystem", "AIModule", "UMG" });

        // https://stackoverflow.com/questions/72379796/how-to-change-include-default-file-path-for-sub-folders
        PublicIncludePaths.AddRange(
        new string[] {
            "CombatTest"
        });
    }
}
