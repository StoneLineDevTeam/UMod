// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

//#define UE4_UMOD_PROFILING //Activate this when you need to do some CPU / memory profiling using UnrealFrontend (that was a test and it's not that amazing

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

#include "OS/UModPlatformUtils.h"
#include "Game/SurfaceTypes.h"

#include "VUI/VUIMain.h"

DECLARE_LOG_CATEGORY_EXTERN(UMod_Game, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(UMod_Maps, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(UMod_Input, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(UMod_Lua, Log, All);

#define CHANNEL_VOICE static_cast<EChannelType>(5) //UMod voice channel (I know it's a bit hacky, but UE4 does not provide an easier way to create a new channel)

//UMod profiling hooks
#ifdef UE4_UMOD_PROFILING

#define UMOD_STAT(Name) \
SCOPE_CYCLE_COUNTER(STAT_UMOD_##Name); \

#define UMOD_STAT_CYLCE(Name, Group) \
DECLARE_CYCLE_STAT(TEXT(#Name), STAT_UMOD_##Group##Name, STAT_UMOD_GROUP_##Group); \

#define UMOD_STAT_GROUP(Name) \
DECLARE_STATS_GROUP(TEXT("UMod:##Name"), STAT_UMOD_GROUP_##Name, STATCAT_Advanced); \

UMOD_STAT_GROUP(LOADING);
UMOD_STAT_CYLCE(Module, LOADING);
UMOD_STAT_CYLCE(Engine, LOADING);
UMOD_STAT_GROUP(PHYSICS);
UMOD_STAT_CYLCE(Sync, PHYSICS);
UMOD_STAT_CYLCE(PhysObj, PHYSICS);
UMOD_STAT_GROUP(LUA);
UMOD_STAT_CYLCE(Load, LUA);
UMOD_STAT_CYLCE(ErrorHandler, LUA);
UMOD_STAT_CYLCE(FuncCall0, LUA); //0 param
UMOD_STAT_CYLCE(FuncCall1, LUA); //1 param
UMOD_STAT_CYLCE(FuncCall2, LUA); //2 params
UMOD_STAT_CYLCE(FuncCall3, LUA); //3 params
UMOD_STAT_GROUP(RENDER);
UMOD_STAT_CYLCE(HUD, RENDER);
#else

//Intentional...
#define UMOD_STAT(Name)

#endif
//End

#include "Entities/Entity.h"
#include "GameFramework/Actor.h"

//Entity registry struct
struct FUModEntityClass {
	FString EntClass;
	UClass *UEClass;
	bool LuaSpawn;

	FUModEntityClass(FString Class, UClass *cl, bool lua) {
		EntClass = Class;
		UEClass = cl;
		LuaSpawn = lua;
	}

	virtual Entity* CastToEntity(AActor *Act) { return NULL; }

	void InfoMessage();
};

//Static array to represent entities
static TMap<FString, FUModEntityClass*> EntityClasses;
static TMap<UClass*, FUModEntityClass*> EntityClassFromUE4;

namespace UModCasts {
	FORCEINLINE Entity *FromActor(AActor *act)
	{
		if (act == NULL) {			
			return NULL;
		}
		FUModEntityClass **Class = EntityClassFromUE4.Find(act->GetClass());
		if (Class == NULL) {
			Class = EntityClassFromUE4.Find(act->GetClass()->GetSuperClass());
		}		
		if (Class != NULL) {
			return (*Class)->CastToEntity(act);
		}
		return NULL;
	}
};

//Macros
#define DEFINE_ENTITY(ClassName, UClassN) \
struct FUModEntityClass_##ClassName : FUModEntityClass { \
	FUModEntityClass_##ClassName(FString Class, UClass *cl, bool lua) : FUModEntityClass(Class, cl, lua) {} \
	virtual Entity *CastToEntity(AActor *act) \
	{ \
		UClassN *CompatibleAct = static_cast<UClassN*>(act); \
		return static_cast<Entity*>(CompatibleAct); \
	} \
}; \
struct Initializer##ClassName { \
	Initializer##ClassName() { \
		FUModEntityClass *Class = new FUModEntityClass_##ClassName(#ClassName, UClassN::StaticClass(), true); \
		EntityClasses.Add(#ClassName, Class); \
		EntityClassFromUE4.Add(UClassN::StaticClass(), Class); \
	} \
}; \
Initializer##ClassName Init##ClassName; \

#define DEFINE_CAST(ClassName, CPPClass) \
namespace UModCasts { \
FORCEINLINE CPPClass *CastTo##ClassName(Entity *Ent) \
{ \
	if (Ent->GetClass() != #ClassName) { \
		return NULL; \
	} \
	return static_cast<CPPClass*>(Ent); \
} \
}; \

#define DEFINE_ENTITY_NO_LUA(ClassName, UClassN) \
struct FUModEntityClass_##ClassName : FUModEntityClass { \
	FUModEntityClass_##ClassName(FString Class, UClass *cl, bool lua) : FUModEntityClass(Class, cl, lua) {} \
	virtual Entity *CastToEntity(AActor *act) \
	{ \
		UClassN *CompatibleAct = Cast<UClassN>(act); \
		return static_cast<Entity*>(CompatibleAct); \
	} \
}; \
struct Initializer##ClassName { \
	Initializer##ClassName() { \
		FUModEntityClass *Class = new FUModEntityClass_##ClassName(#ClassName, UClassN::StaticClass(), false); \
		EntityClasses.Add(#ClassName, Class); \
		EntityClassFromUE4.Add(UClassN::StaticClass(), Class); \
	} \
}; \
Initializer##ClassName Init##ClassName; \

#define DECLARE_UMOD_COMMAND(id, name, help, needcxt, fnc) \
struct InitializerCMD##id { \
	InitializerCMD##id() \
	{ \
		UUModConsoleManager::ConsoleCommands[id] = new FUModConsoleCommand(name, help, needcxt, fnc); \
		UUModConsoleManager::ConsoleCommandNumber = UUModConsoleManager::ConsoleCommandNumber + 1; \
	} \
}; \
InitializerCMD##id InitCMD##id; \
//End

//Config files
static const FString InputCFG = FPaths::GameSavedDir() + FString("Config/UMod.Input.cfg");
static const FString ClientCFG = FPaths::GameSavedDir() + FString("Config/UMod.Client.cfg");
static const FString ServerCFG = FPaths::GameSavedDir() + FString("Config/UMod.Server.cfg");
//End

//Versions
static const FString GVersion = FString("0.4 - Alpha");
const FString LuaEngineVersion = FString("0.1 - UNSTABLE");
//End

//Usefull common functions
template <typename ObjClass>
static FORCEINLINE ObjClass* LoadObjFromPath(const FName& Path)
{
	if (Path == NAME_None) return NULL;
	return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), NULL, *Path.ToString()));
}

static FORCEINLINE FString StringFromBool(bool b)
{
	return b ? "True" : "False";
}
//End

//Game Module interface
class FUModGameModule : public FDefaultGameModuleImpl
{	
	virtual void StartupModule() override;
	virtual void ShutdownModule();
};
//End

#endif
