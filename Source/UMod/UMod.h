// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#ifndef __UMOD_H__
#define __UMOD_H__

#include "Engine.h"

//NET
#include "UnrealNetwork.h"
#include "Online.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

//UMG
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

#include "ParticleDefinitions.h"

DECLARE_LOG_CATEGORY_EXTERN(UMod_Game, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(UMod_Maps, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(UMod_Input, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(UMod_Lua, Log, All);

#define CHANNEL_VOICE static_cast<EChannelType>(5) //UMod voice channel (I know it's a bit hacky, but UE4 does not provide an easier way to create a new channel)

//Static array to represent entities
static TMap<FString, UClass*> EntityClasses;

#define DEFINE_ENTITY(ClassName, UClassPtr) \
struct Initializer##ClassName { \
	Initializer##ClassName() { \
		UE_LOG(UMod_Game, Log, TEXT("Registering Entity %s"), *FString(#ClassName)); \
		EntityClasses.Add(#ClassName, UClassPtr); \
	} \
}; \
Initializer##ClassName Init##ClassName; \

static const FString InputCFG = FPaths::GameConfigDir() + FString("UMod.Input.cfg");
static const FString ClientCFG = FPaths::GameConfigDir() + FString("UMod.Client.cfg");
static const FString ServerCFG = FPaths::GameConfigDir() + FString("UMod.Server.cfg");

template <typename ObjClass>
static FORCEINLINE ObjClass* LoadObjFromPath(const FName& Path)
{
	if (Path == NAME_None) return NULL;
	//~

	return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), NULL, *Path.ToString()));
}

static FORCEINLINE FString GetObjPath(UObject* obj)
{
	FStringAssetReference path = FStringAssetReference(obj);
	if (!path.IsValid()) { return FString(); }
	return path.ToString().Mid(6, path.ToString().Len());
}

static FORCEINLINE FString StringFromBool(bool b)
{
	return b ? "True" : "False";
}

class FUModGameModule : public FDefaultGameModuleImpl
{	
	virtual void StartupModule() override;
	virtual void ShutdownModule();

private:
	void *LuaDLLHandle;
};

#endif
