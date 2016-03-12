// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UMod.h"


IMPLEMENT_PRIMARY_GAME_MODULE(FUModGameModule, UMod, "UMod");
 
DEFINE_LOG_CATEGORY(UMod_Game);
DEFINE_LOG_CATEGORY(UMod_Maps);
DEFINE_LOG_CATEGORY(UMod_Input);
DEFINE_LOG_CATEGORY(UMod_Lua);

void FUModGameModule::StartupModule()
{
/*	UE_LOG(UMod_Game, Log, TEXT("Installing LUA binaries..."));

	FString LuaPath = FPaths::GameDir() + FString("ThirdParty/Lua/");
	FString LuaDllFile = FPaths::GameDir() + FString("ThirdParty/Lua/");
#if PLATFORM_WINDOWS
	#if PLATFORM_32BITS
	LuaPath += FString("Win32/");
	LuaDllFile += FString("Win32/lua53.dll");
	#elif PLATFORM_64BITS
	LuaPath += FString("Win64/");
	LuaDllFile += FString("Win64/lua53.dll");
	#endif
#elif PLATFORM_MAC
	LuaPath += FString("MacOS/");
	LuaDllFile += FString("MacOS/lua53.a");
#elif PLATFORM_LINUX
	LuaPath += FString("Linux/");
	LuaDllFile += FString("Linux/lua53.a");
#endif

	UE_LOG(UMod_Game, Log, TEXT("LuaPath = %s"), *LuaPath);
	UE_LOG(UMod_Game, Log, TEXT("LuaDllFile = %s"), *LuaDllFile);

	FPlatformProcess::PushDllDirectory(*LuaPath);
	LuaDLLHandle = FPlatformProcess::GetDllHandle(*LuaDllFile);
	FPlatformProcess::PopDllDirectory(*LuaPath);*/
}

void FUModGameModule::ShutdownModule()
{
	/*if (LuaDLLHandle != NULL) {
		FPlatformProcess::FreeDllHandle(LuaDLLHandle);
	}*/
}