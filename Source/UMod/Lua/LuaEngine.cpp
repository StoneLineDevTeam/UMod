// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "LuaEngine.h"
#include "UModGameInstance.h"
#include "Renderer/Render2D.h"
#include "LuaLibSurface.h"

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
/*End*/

/*Base log.* library*/
static int LogInfo(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FString msg = Lua.CheckString(-1);
	UE_LOG(UMod_Lua, Log, TEXT("%s"), *msg);
	return 0;
}
static int LogWarn(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FString msg = Lua.CheckString(-1);
	UE_LOG(UMod_Lua, Warning, TEXT("%s"), *msg);
	return 0;
}
static int LogErr(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FString msg = Lua.CheckString(-1);
	UE_LOG(UMod_Lua, Error, TEXT("%s"), *msg);
	return 0;
}
/*End*/

/*Base game.* library*/
static int GameGetMaps(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	Lua.NewTable();
	TArray<FUModMap> Maps = Game->AssetsManager->GetMapList();
	UE_LOG(UMod_Lua, Warning, TEXT("[DEBUG]Available maps : %i"), Maps.Num());
	for (int i = 0; i < Maps.Num(); i++) {
		FUModMap Map = Maps[i];
		//Reverse the index so now lua can easely do #tbl or for k, v in pairs(tbl)
		Lua.PushInt(i + 1);
		//Create a sub table that contains all data
		Lua.NewTable();
		Lua.PushString("NiceName");
		Lua.PushString(Map.NiceName);
		Lua.SetTable(-3);
		Lua.PushString("Path");
		Lua.PushString(Map.Path);
		Lua.SetTable(-3);
		Lua.PushString("Category");
		Lua.PushString(Map.Category);
		Lua.SetTable(-3);
		//Add the new sub table to the large one
		Lua.SetTable(-3);
	}
	return 1;
}
static int GameGetAssets(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	int i = Lua.CheckInt(-1);
	Lua.NewTable();
	UE_LOG(UMod_Lua, Warning, TEXT("[DEBUG]Asset type to list : %i"), i);
	EUModAssetType t = EUModAssetType(i);
	TArray<FUModAsset> Assets = Game->AssetsManager->GetAssetList(t);
	for (int i = 0; i < Assets.Num(); i++) {
		FUModAsset Asset = Assets[i];
		//Reverse the index so now lua can easely do #tbl or for k, v in pairs(tbl)
		Lua.PushInt(i + 1);
		//Create a sub table that contains all data
		Lua.NewTable();
		Lua.PushString("NiceName");
		Lua.PushString(Asset.NiceName);
		Lua.SetTable(-3);
		Lua.PushString("Path");
		Lua.PushString(Asset.Path);
		Lua.SetTable(-3);
		//Add the new sub table to the large one
		Lua.SetTable(-3);
	}
	return 1;
}
static int GameIsDedicated(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	Lua.PushBool(Game->IsDedicatedServer());
	return 1;
}
static int GameDisconnect(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FString msg = Lua.CheckString(-1);
	Game->Disconnect(msg);
	return 0;
}
static int GameShowFatalMessage(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FString msg = Lua.CheckString(-1);
	UUModGameInstance::ShowFatalMessage(msg);
	return 0;
}
static int GameExit(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	UUModGameInstance::ExitGame();
	return 0;
}
/*End*/

LuaEngine::LuaEngine(UUModGameInstance *g)
{
	if (g->IsEditor()) {
		UE_LOG(UMod_Lua, Error, TEXT("Lua did not initialize as running lua in editor is not allowed !"));
		return;
	}
	Game = g;
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
	//Remove debug lib (I believe it's useless in UMod environment)
	Lua->PushNil();
	Lua->SetGlobal("debug");
	//Remove os lib (will replace by a system lib which will bridge with UE4)
	Lua->PushNil();
	Lua->SetGlobal("os");
	//Add Color() function
	Lua->PushCFunction(Color);
	Lua->SetGlobal("Color");

	//Custom UMod libs
	BeginLibReg("log");
	AddLibFunction("Info", LogInfo);
	AddLibFunction("Warning", LogWarn);
	AddLibFunction("Error", LogErr);
	CreateLibrary();
	BeginLibReg("game");
	AddLibFunction("GetMapList", GameGetMaps);
	AddLibFunction("GetAssetList", GameGetAssets);
	AddLibFunction("IsDedicated", GameIsDedicated);
	AddLibFunction("Disconnect", GameDisconnect);
	AddLibFunction("ShowFatalMessage", GameShowFatalMessage);
	CreateLibrary();
	LuaLibSurface::RegisterSurfaceLib(this);

	//Enums
	BeginLibReg("AssetType");
	AddLibConstant("MATERIAL", 0);
	AddLibConstant("TEXTURE", 1);
	AddLibConstant("MODEL", 2);
	CreateLibrary();
	BeginLibReg("DrawEnums");
	AddLibConstant("TEXT_ALIGN_CENTER", 1);
	AddLibConstant("TEXT_ALIGN_LEFT", 0);
	AddLibConstant("TEXT_ALIGN_RIGHT", 2);
	CreateLibrary();
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

void LuaEngine::RunScriptFunction(ETableType Tbl, uint8 resultNumber, uint8 ArgNum, FString FuncName, ...)
{
	va_list vl;
	va_start(vl, FuncName);

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

	for (int i = 0; i < ArgNum; i++) {
		const char c = va_arg(vl, const char);
		UE_LOG(UMod_Lua, Warning, TEXT("RunScriptFunction.c=%i"), (int)c);
		switch (c) {
		case 'd'://double
			Lua->PushNum(va_arg(vl, double));
			break;
		case 'i'://integer
			Lua->PushInt(va_arg(vl, int));
			break;
		case 's'://string
			Lua->PushString(va_arg(vl, FString));
			break;
		case 'b'://boolean
			Lua->PushBool(va_arg(vl, bool));
			break;
		case 'f'://float
			float f = va_arg(vl, float);
			Lua->PushNum((double)f);
			break;
		}
	}

	ELuaErrorType t = Lua->PCall(ArgNum, resultNumber, 0);
	if (t != ELuaErrorType::NONE) {
		FString msg = Lua->ToString(-1);
		HandleLuaError(t, msg);
	}
}

FString LuaEngine::GetLuaVersion()
{
	return Lua->GetVersion();
}