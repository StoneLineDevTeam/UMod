// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "LuaEngine.h"
#include "UModGameInstance.h"
#include "Renderer/Render2D.h"
#include "Renderer/Render3D.h"

#include "Lib.h"

#include "LibsInclude.h"

#include "Entities/EntityBase.h"

//TArray<LuaLib*> LuaEngine::LibRegistry;

/*Base replacement methods*/
static int Print(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FString msg = Lua.ToString(-1);
	UE_LOG(UMod_Lua, Warning, TEXT("Lua->Print:%s"), *msg);
	return 0;
}
static int Type(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	ELuaType t = Lua.GetType(-1);
	Lua.PushInt((int)t);
	return 1;
}
/*End*/

/*Add render lib*/
static int RenderCreate3D2DTarget(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	uint32 W = (uint32)Lua.CheckInt(1);
	uint32 H = (uint32)Lua.CheckInt(2);
	uint8 id = URender3D::Create3D2DTarget(W, H);
	Lua.PushInt((int)id);
	return 1;
}
static int RenderRender3D2DTarget(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	uint8 id = (uint8)Lua.CheckInt(1);
	URender3D::Render3D2DTarget(id);
	return 0;
}
/*End*/

LuaEngine::LuaEngine(UUModGameInstance *g)
{
	UMOD_STAT(LUALoad);

	Lua = LuaInterface::New();
	if (Lua == NULL) {
		UE_LOG(UMod_Lua, Error, TEXT("Lua failed to initialize !"));
		return;
	}
	
	Lua->OpenLibs();

	//Overwrite print function
	Lua->GetGlobal("_G");
	Lua->PushString("print");
	Lua->PushCFunction(Print);
	Lua->SetTable(-3);
	//Overwrite type function
	Lua->PushString("type");
	Lua->PushCFunction(Type);
	Lua->SetTable(-3);
	//Remove loadfile/dofile functions of Lua
	Lua->PushString("loadfile");
	Lua->PushNil();
	Lua->SetTable(-3);
	Lua->PushString("dofile");
	Lua->PushNil();
	Lua->SetTable(-3);
	//Remove io lib (don't worry this will be replaced)
	Lua->PushNil();
	Lua->SetGlobal("io");
	//Remove package lib (removed as I don't even know what it is)
	Lua->PushNil();
	Lua->SetGlobal("package");
	//Remove os lib (will replace by a system lib which will bridge with UE4)
	Lua->PushNil();
	Lua->SetGlobal("os");
	
	//Custom UMod libs
	if (!g->IsDedicatedServer()) {
		BeginLibReg("render");
		AddLibFunction("Create3D2DTarget", RenderCreate3D2DTarget);
		AddLibFunction("Render3D2DTarget", RenderRender3D2DTarget);
		CreateLibrary();
	}

	LuaLib::Game = g;

	UE_LOG(UMod_Lua, Log, TEXT("[DEBUG]Registered %i Lua API classes"), LibRegistry.Num());
	for (int i = 0; i < LibRegistry.Num(); i++) {
		LuaLib *Lib = LibRegistry[i];
		if (Lib->IsClientOnly()) {
			if (!g->IsDedicatedServer()) {				
				Lib->RegisterLib(this);
			}
		} else {			
			Lib->RegisterLib(this);
		}
	}

	//Global MetaTables
	LuaEntity::RegisterEntityMetaTable(Lua);
	LuaEntity::RegisterPlayerMetaTable(Lua);

	//Enums
	BeginLibReg("Type");
	AddLibConstant("TABLE", 0);
	AddLibConstant("STRING", 1);
	AddLibConstant("NUMBER", 2);
	AddLibConstant("BOOL", 3);
	AddLibConstant("ENTITY", 4);
	AddLibConstant("COLOR", 5);
	AddLibConstant("VECTOR", 6);
	AddLibConstant("ANGLE", 7);
	AddLibConstant("UNKNOWN", 8);
	AddLibConstant("NIL", 9);
	CreateLibrary();

	//Add the GM (GameMode) global table
	Lua->NewTable();
	Lua->SetGlobal("GM");

	//Again a test failure
	Lua->Register("LUA_InternalErrorHandler", LUA_InternalErrorHandler);
}

LuaEngine::~LuaEngine()
{	
	Lua->Close();
}

void LuaEngine::AddClassToRegistry(LuaLib *Lib)
{
	LibRegistry.Add(Lib);
}

/*Lib reg*/
void LuaEngine::BeginLibReg(FString libName)
{
	Lua->NewTable();
	CurLibName = libName;
}
void LuaEngine::AddLibFunction(FString name, lua_CFunction func)
{
	Lua->PushString(name);
	Lua->PushCFunction(func);
	Lua->SetTable(-3);
}
void LuaEngine::AddLibConstant(FString name, int i)
{
	Lua->PushString(name);
	Lua->PushInt(i);
	Lua->SetTable(-3);
}
void LuaEngine::CreateLibrary()
{
	Lua->SetGlobal(CurLibName);
	CurLibName = FString();
}
/*End*/

void LuaEngine::HandleLuaError(ELuaErrorType t, FString msg, TArray<FString> &trace)
{
	if (t == ELuaErrorType::PARSER) {
		UE_LOG(UMod_Lua, Error, TEXT("LuaScript syntax error :"));
		UE_LOG(UMod_Lua, Error, TEXT("      %s"), *msg);
	} else if (t == ELuaErrorType::RUNTIME) {
		UE_LOG(UMod_Lua, Error, TEXT("LuaScript runtime error : %s"), *msg);
		for (int i = 1; i < trace.Num(); i++) {
			//Remove first 20 characters from file path
			FString stackElem = trace[i];
			stackElem = stackElem.Replace(TEXT("../"), TEXT(""));
			UE_LOG(UMod_Lua, Error, TEXT("      %s"), *stackElem);
		}		
	}	
}

void LuaEngine::RunScript(FString path)
{
	TArray<FString> empty;
	ELuaErrorType type = Lua->LoadFile(path);
	if (type != ELuaErrorType::NONE) {
		FString str = Lua->ToString(-1);
		HandleLuaError(type, str, empty);
	}
	ELuaErrorType t = Lua->PCall(0, 0, 0);
	if (t != ELuaErrorType::NONE) {		
		FString str = Lua->ToString(-1);
		HandleLuaError(t, str, empty);
	}
}

void LuaEngine::InitGameMode()
{
	Lua->GetGlobal("LUA_InternalErrorHandler");

	Lua->GetGlobal("GM");
	Lua->PushString("Initialize");
	Lua->GetTable(-2);

	ELuaErrorType t = Lua->PCall(0, 0, -3);
	if (t != ELuaErrorType::NONE) {
		GameModeInitialized = false;
		UE_LOG(UMod_Lua, Error, TEXT("An error has occured while initializing GameMode '%s'"), *LuaLib::Game->GetGameMode());
	} else {
		GameModeInitialized = true;
	}
}

bool LuaEngine::RunScriptFunctionZeroParam(ETableType Tbl, uint8 resultNumber, FString FuncName)
{
	UMOD_STAT(LUAFuncCall0);

	if (!GameModeInitialized) {
		return false;
	}

	Lua->GetGlobal("LUA_InternalErrorHandler");

	switch (Tbl) {
	case GLOBAL:
		Lua->GetGlobal("_G");
		break;
	case GAMEMODE:
		Lua->GetGlobal("GM");
		break;
	}
	Lua->PushString(FuncName);
	Lua->GetTable(-2);

	ELuaErrorType t = Lua->PCall(0, resultNumber, -3);
	if (t != ELuaErrorType::NONE) {
		return false;
	}
	return true;
}

FString LuaEngine::GetLuaVersion()
{
	return Lua->GetVersion();
}