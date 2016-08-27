// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UModEditor : ModuleRules
{
    public UModEditor(TargetInfo Target)
    {
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystem", "OnlineSubsystemUtils", "UMG", "Slate", "SlateCore", "Networking", "Sockets", "PakFile", "RHI", "RenderCore", "ShaderCore", "EngineSettings", "Messaging", "EngineMessages", "UMod", "UnrealEd", "MainFrame", "ContentBrowser" });

        PrivateDependencyModuleNames.AddRange(new string[] { "DesktopPlatform", "AssetTools" });

        DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");
    }
}
