// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class UMod : ModuleRules
{
    public const bool USE_LUA_DEV_BUILD = false;

	public UMod(TargetInfo Target)
	{        
        PublicDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine", "InputCore", "OnlineSubsystem", "OnlineSubsystemUtils", "UMG", "Slate", "SlateCore", "Networking", "Sockets", "PakFile", "RHI", "RenderCore", "ShaderCore", "EngineSettings", "Messaging", "EngineMessages"});

        DynamicallyLoadedModuleNames.Add("OnlineSubsystemNull");
        DynamicallyLoadedModuleNames.Add("AssetRegistry");

        Console.WriteLine("--> Injecting Lua Library <--");
        Console.WriteLine("ThirdParty path is " + ThirdPartyPath);
        bool b = InstallLua(Target);
        if (b)
        {
            Console.WriteLine("Successfully installed Lua !");
        }
        else
        {
            Console.WriteLine("Error installing Lua !");
        }
        Console.WriteLine("--> End <--");
	}

    private string ModulePath
    {
        get { return ModuleDirectory; }
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
                return Path.Combine(ThirdPartyPath, "Lua/Win64/");
            case UnrealTargetPlatform.Win32:
                return Path.Combine(ThirdPartyPath, "Lua/Win32/");
            case UnrealTargetPlatform.Mac:
                return Path.Combine(ThirdPartyPath, "Lua/MacOS/");
            case UnrealTargetPlatform.Linux:
                return Path.Combine(ThirdPartyPath, "Lua/Linux/");
            default:
                return null;
        }
    }

    private string GetLuaStaticLib(UnrealTargetPlatform platform)
    {
        switch (platform)
        {
            case UnrealTargetPlatform.Win64:
                return "lua53.lib";
            case UnrealTargetPlatform.Win32:
                return "lua53.lib";
            case UnrealTargetPlatform.Mac:
                return "lua53.a";
            case UnrealTargetPlatform.Linux:
                return "lua53.a";
            default:
                return null;
        }
    }

    private string GetLuaDynamicLib(UnrealTargetPlatform platform)
    {
        switch (platform)
        {
            case UnrealTargetPlatform.Win64:
                return "lua53.dll";
            case UnrealTargetPlatform.Win32:
                return "lua53.dll";
            case UnrealTargetPlatform.Mac:
                return "lua53.dylib";
            case UnrealTargetPlatform.Linux:
                return "lua53.so";
            default:
                return null;
        }
    }

    private bool InstallLua(TargetInfo Target)
    {
        string LuaIncludePath = Path.Combine(ThirdPartyPath, "Lua/Includes");
        string LuaPath = GetLuaPath(Target.Platform);
        string LuaDL = GetLuaDynamicLib(Target.Platform);
        string LuaSL = GetLuaStaticLib(Target.Platform);
        if (Target.Platform == UnrealTargetPlatform.Win64 && UEBuildConfiguration.bBuildEditor && USE_LUA_DEV_BUILD)
        {
            LuaDL = "lua53_apicheck.dll";
            LuaSL = "lua53_apicheck.lib";
        }        
        if (LuaPath != null)
        {
            Console.WriteLine("LuaPath is : " + LuaPath);
            Console.WriteLine("LuaDynamicLib : " + LuaDL + ", LuaStaticLib : " + LuaSL);
            PublicLibraryPaths.Add(LuaPath);
            PublicAdditionalLibraries.Add(LuaSL);
            PublicDelayLoadDLLs.Add(LuaDL); //Tried to do something but it still needs some C++ hacky ways (engine source code change)...
            PublicIncludePaths.Add(LuaIncludePath);            
            return true;
        }
        return false;
    }
}
