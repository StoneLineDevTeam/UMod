// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "LuaEngine.h"
#include "UModGameInstance.h"
#include "Renderer/Render2D.h"
#include "Renderer/Render3D.h"
#include "LuaLibSurface.h"
#include "LuaLibLog.h"
#include "LuaLibGame.h"

static UUModGameInstance *Game;

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

/*Global methods*/
static int Color(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	int r = Lua.CheckInt(-4);
	int g = Lua.CheckInt(-3);
	int b = Lua.CheckInt(-2);
	int a = Lua.CheckInt(-1);
	Lua.PushColor(FColor(r, g, b, a));
	return 1;
}
static int Include(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FString ToInclude = Lua.CheckString(-1);
	if (Game->IsDedicatedServer() || Game->IsListenServer()) { //We are a server
		Game->Lua->RunScript(ToInclude);
	} else { //We are a client
		FString real = Game->AssetsManager->GetLuaFile(ToInclude);
		Game->Lua->RunScript(real);
	}
	return 0;
}
static int AddCSLuaFile(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FString ToAdd = Lua.CheckString(-1);
	Game->AssetsManager->AddSVLuaFile(FPaths::GameDir() + ToAdd, ToAdd);
	UE_LOG(UMod_Lua, Log, TEXT("Added lua file for upload : '%s'."), *ToAdd);
	return 0;
}
static int HasAuthority(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	bool b = Game->IsDedicatedServer() || Game->IsListenServer();
	Lua.PushBool(b);
	return 1;
}
/*End*/

/*Add render lib*/
//FUCK YOU MOTHER FUCKING GITHUB
static int RenderCreate3D2DTarget(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	uint32 W = (uint32)Lua.CheckInt(-2);
	uint32 H = (uint32)Lua.CheckInt(-1);
	uint8 id = URender3D::Create3D2DTarget(W, H);
	Lua.PushInt((int)id);
	return 1;
}
static int RenderRender3D2DTarget(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	uint8 id = (uint8)Lua.CheckInt(-1);
	URender3D::Render3D2DTarget(id);
	return 0;
}

/*End*/

LuaEngine::LuaEngine(UUModGameInstance *g)
{
	Game = g;
	Lua = LuaInterface::New();
	if (Lua == NULL) {
		UE_LOG(UMod_Lua, Error, TEXT("Lua failed to initialize !"));
		return;
	}

	//Leave this block here so we have a LuaInterface and LuaEngine running but empty in editor
	if (g->IsEditor()) {
		UE_LOG(UMod_Lua, Error, TEXT("Lua did not initialize as running lua in editor is not allowed !"));
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
	//Remove debug lib (I believe it's useless in UMod environment)
	Lua->PushNil();
	Lua->SetGlobal("debug");
	//Remove os lib (will replace by a system lib which will bridge with UE4)
	Lua->PushNil();
	Lua->SetGlobal("os");
	//Add Color() function
	Lua->PushCFunction(Color);
	Lua->SetGlobal("Color");
	//Add Include/AddCSLuaFile functions
	Lua->PushCFunction(Include);
	Lua->SetGlobal("Include");
	Lua->PushCFunction(AddCSLuaFile);
	Lua->SetGlobal("AddCSLuaFile");
	//Add HasAuthority function
	Lua->PushCFunction(HasAuthority);
	Lua->SetGlobal("HasAuthority");

	//Custom UMod libs
	LuaLibGame::RegisterGameLib(this, Game);
	LuaLibLog::RegisterLogLib(this);
	if (!g->IsDedicatedServer()) {
		LuaLibSurface::RegisterSurfaceLib(this);
		BeginLibReg("render");
		AddLibFunction("Create3D2DTarget", RenderCreate3D2DTarget);
		AddLibFunction("Render3D2DTarget", RenderRender3D2DTarget);
		CreateLibrary();
	}

	//Enums
	BeginLibReg("Type");
	AddLibConstant("TABLE", 0);
	AddLibConstant("STRING", 1);
	AddLibConstant("NUMBER", 2);
	AddLibConstant("BOOL", 3);
	AddLibConstant("ENTITY", 4);
	AddLibConstant("COLOR", 5);
	AddLibConstant("VECTOR", 6);
	AddLibConstant("UNKNOWN", 7);
	AddLibConstant("NIL", 8);
	CreateLibrary();

	//Add the GM (GameMode) global table
	Lua->NewTable();
	Lua->SetGlobal("GM");
}

LuaEngine::~LuaEngine()
{
	Lua->Close();
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

void LuaEngine::HandleLuaError(ELuaErrorType t, FString msg)
{
	if (t == ELuaErrorType::PARSER) {
		UE_LOG(UMod_Lua, Error, TEXT("LuaScript syntax error :"));
		UE_LOG(UMod_Lua, Error, TEXT("      %s"), *msg);
	} else if (t == ELuaErrorType::RUNTIME) {
		UE_LOG(UMod_Lua, Error, TEXT("LuaScript runtime error :"));
		UE_LOG(UMod_Lua, Error, TEXT("      %s"), *msg);
	}
}

void LuaEngine::RunScript(FString path)
{
	ELuaErrorType type = Lua->LoadFile(path);
	if (type != ELuaErrorType::NONE) {
		FString str = Lua->ToString(-1);
		HandleLuaError(type, str);
	}
	ELuaErrorType t = Lua->PCall(0, 0, 0);
	if (t != ELuaErrorType::NONE) {		
		FString str = Lua->ToString(-1);
		HandleLuaError(t, str);
	}
}

void LuaEngine::RunScriptFunctionZeroParam(ETableType Tbl, uint8 resultNumber, FString FuncName)
{
	Lua->TraceBack(-1);

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

	ELuaErrorType t = Lua->PCall(0, resultNumber, 1);
	if (t != ELuaErrorType::NONE) {
		FString msg = Lua->ToString(-1);
		HandleLuaError(t, msg);
	}
}

FString LuaEngine::GetLuaVersion()
{
	return Lua->GetVersion();
}