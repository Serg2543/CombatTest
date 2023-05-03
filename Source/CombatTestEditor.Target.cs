// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class CombatTestEditorTarget : TargetRules
{
	public CombatTestEditorTarget(TargetInfo Target) : base(Target)
	{
        bEnforceIWYU = true;
        //bUseUnityBuild = true;
        bUseUnityBuild = true; //  false
        bUseIncrementalLinking = false;

        Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("CombatTest");
	}
}
