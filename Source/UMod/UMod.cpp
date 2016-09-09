// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UMod.h"
#include "UModGameEngine.h"
#include <iostream>
#include <functional>


IMPLEMENT_PRIMARY_GAME_MODULE(FUModGameModule, UMod, "UMod");
 
DEFINE_LOG_CATEGORY(UMod_Game);
DEFINE_LOG_CATEGORY(UMod_Maps);
DEFINE_LOG_CATEGORY(UMod_Input);
DEFINE_LOG_CATEGORY(UMod_Lua);

void FUModEntityClass::InfoMessage()
{
	UE_LOG(UMod_Game, Log, TEXT("UMod Entity [ Class='%s' | CClass='%s' | LuaSpawn='%s' ]"), *EntClass, *UEClass->GetName(), *StringFromBool(LuaSpawn));
}

void FUModGameModule::StartupModule()
{
	UMOD_STAT(LOADINGModule);

	UE_LOG(UMod_Game, Log, TEXT("--> UMod - Program Startup <--"));
		
	TArray<FString> args;
	FString s = FCommandLine::Get();
	s.ParseIntoArray(args, TEXT(" "));
	
	//Trying to deconstruct UE4 window init shit
	if (!GIsEditor && !args.Contains("-server")) {
		int width;
		int height;
		bool full;
		UUModGameEngine::GetDisplayProperties(width, height, full);		
		FString begin;
		if (args.Contains("-game")) {
			begin = "-game";
		}
		begin += " -ResX=" + FString::FromInt(width);
		begin += " -ResY=" + FString::FromInt(height);
		begin += " -FORCERES";
		if (full) {
			begin += " -FULLSCREEN";
		} else {
			begin += " -WINDOWED";
		}
#ifdef UE4_UMOD_PROFILING
		begin += "-messaging";
		FString UFrontEndExe = FPaths::EngineDir() + "/Binaries/Win64/UnrealFrontend.exe";
		FString Params = "-messaging";
		FPlatformProcess::CreateProc(*UFrontEndExe, *Params, true, false, false, NULL, 0, NULL, NULL);
#endif
		FCommandLine::Set(*begin);

		UE_LOG(UMod_Game, Log, TEXT("Deconstructed UE4 Window init, New CMD '%s'"), FCommandLine::Get());
		UE_LOG(UMod_Game, Log, TEXT("Have fun UE4, you ate the resolution by your ASS, HEHE !!"));
	}
	UE_LOG(UMod_Game, Log, TEXT("Entity registry dump start"));
	for (TPair<UClass*, FUModEntityClass*> Elem : EntityClassFromUE4) {
		Elem.Value->InfoMessage();
	}
	UE_LOG(UMod_Game, Log, TEXT("Entity registry dump end"));

	UE_LOG(UMod_Game, Log, TEXT("--> UMod - End <--"));
}

void FUModGameModule::ShutdownModule()
{
}