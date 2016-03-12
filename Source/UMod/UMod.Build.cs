// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class UMod : ModuleRules
{
	public UMod(TargetInfo Target)
	{
        PublicDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystem", "OnlineSubsystemUtils", "UMG", "Slate", "SlateCore", "Networking", "Sockets", "PakFile", "RHI", "EngineSettings"});

        DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");

        Console.WriteLine(ThirdPartyPath);

        bool b = InstallLua(Target);
        if (b)
        {
            Console.WriteLine("Successfully installed Lua !");
        }
        else
        {
            Console.WriteLine("Error installing Lua !");
        }
	}

    private string ModulePath
    {
        get { return Path.GetDirectoryName(RulesCompiler.GetModuleFilename(this.GetType().Name)); }
    }
 
    private string ThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty")); }
    }

    private string GetLuaPath(UnrealTargetPlatform platform)
    {
        switch (platform)
        {
            case UnrealTargetPlatform.Win64:
                return Path.Combine(ThirdPartyPath, "Lua/Win64/lua53.lib");
            case UnrealTargetPlatform.Win32:
                return Path.Combine(ThirdPartyPath, "Lua/Win32/lua53.lib");
            case UnrealTargetPlatform.Mac:
                return Path.Combine(ThirdPartyPath, "Lua/MacOS/lua53.dylib");
            case UnrealTargetPlatform.Linux:
                return Path.Combine(ThirdPartyPath, "Lua/Linux/lua53.so");
            default:
                return null;
        }
    }

    private string GetLuaDLLPath(UnrealTargetPlatform platform)
    {
        switch (platform)
        {
            case UnrealTargetPlatform.Win64:
                return Path.Combine(ThirdPartyPath, "Lua/Win64/lua53.dll");
            case UnrealTargetPlatform.Win32:
                return Path.Combine(ThirdPartyPath, "Lua/Win32/lua53.dll");
            case UnrealTargetPlatform.Mac:
                return Path.Combine(ThirdPartyPath, "Lua/MacOS/lua53.a");
            case UnrealTargetPlatform.Linux:
                return Path.Combine(ThirdPartyPath, "Lua/Linux/lua53.a");
            default:
                return null;
        }
    }

    private bool InstallLua(TargetInfo Target)
    {
        //PublicIncludePaths
        //PublicAdditionalLibraries
        string LuaIncludePath = Path.Combine(ThirdPartyPath, "Lua/Includes");
        string LuaPath = GetLuaPath(Target.Platform);
        string LuaDLLPath = GetLuaDLLPath(Target.Platform);
        if (LuaPath != null)
        {            
            Console.WriteLine("LuaPath is : " + LuaPath);
            Console.WriteLine("LuaDLLPath is : " + LuaDLLPath);
            PublicAdditionalLibraries.Add(LuaPath);
            PublicDelayLoadDLLs.Add(LuaDLLPath); //Tried to do something but it still needs some C++ hacky ways (engine source code change)...
            PublicIncludePaths.Add(LuaIncludePath);            
            return true;
        }        
        return false;
    }
}
