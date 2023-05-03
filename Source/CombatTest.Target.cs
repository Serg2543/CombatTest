// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class CombatTestTarget : TargetRules
{
	public CombatTestTarget(TargetInfo Target) : base(Target)
	{
        bEnforceIWYU = true;
        //bUseUnityBuild = true;
        bUseUnityBuild = true; // false
        bUseIncrementalLinking = false;

        Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("CombatTest");
	}
}
