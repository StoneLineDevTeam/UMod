// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UMod : ModuleRules
{
	public UMod(TargetInfo Target)
	{
        PublicDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystem", "OnlineSubsystemUtils", "UMG", "Slate", "SlateCore", "Networking", "Sockets"});

        DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");
	}
}
