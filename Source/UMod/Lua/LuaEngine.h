// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "Interface/LuaInterface.h"
#include "TypesInclude.h"

class LuaLib;

class AUModCharacter;
class Entity;
template<typename T>
struct FLuaParam {
	T Value;

	FLuaParam(T val) {
		Value = val;
	}

	void Push(LuaInterface* Lua);
	void Check(LuaInterface* Lua, int id);
};
template<>
struct FLuaParam<int> {
	int Value;

	FLuaParam(int val) {
		Value = val;
	}

	void Push(LuaInterface* Lua)
	{
		Lua->PushInt(Value);
	}
	void Check(LuaInterface* Lua, int id)
	{
		Value = Lua->CheckInt(id);
	}
};
template<>
struct FLuaParam<float> {
	float Value;

	FLuaParam(float val) {
		Value = val;
	}

	void Push(LuaInterface* Lua)
	{
		Lua->PushFloat(Value);
	}
	void Check(LuaInterface* Lua, int id)
	{
		Value = Lua->CheckFloat(id);
	}
};
template<>
struct FLuaParam<bool> {
	bool Value;

	FLuaParam(bool val) {
		Value = val;
	}

	void Push(LuaInterface* Lua)
	{
		Lua->PushBool(Value);
	}
	void Check(LuaInterface* Lua, int id)
	{
		Value = Lua->CheckBool(id);
	}
};
template<>
struct FLuaParam<FString> {
	FString Value;

	FLuaParam(FString val) {
		Value = val;
	}

	void Push(LuaInterface* Lua)
	{
		Lua->PushString(Value);
	}
	void Check(LuaInterface* Lua, int id)
	{
		Value = Lua->CheckString(id);
	}
};
template<>
struct FLuaParam<FColor> {
	FColor Value;

	FLuaParam(FColor val) {
		Value = val;
	}

	void Push(LuaInterface* Lua)
	{
		Lua->PushColor(Value);
	}
	void Check(LuaInterface* Lua, int id)
	{
		Value = Lua->CheckColor(id);
	}
};
template<>
struct FLuaParam<FVector> {
	FVector Value;

	FLuaParam(FVector val) {
		Value = val;
	}

	void Push(LuaInterface* Lua)
	{
		Lua->PushVector(Value);
	}
	void Check(LuaInterface* Lua, int id)
	{
		Value = Lua->CheckVector(id);
	}
};
template<>
struct FLuaParam<FRotator> {
	FRotator Value;

	FLuaParam(FRotator val) {
		Value = val;
	}

	void Push(LuaInterface* Lua)
	{
		Lua->PushAngle(Value);
	}
	void Check(LuaInterface* Lua, int id)
	{
		Value = Lua->CheckAngle(id);
	}
};
template<>
struct FLuaParam<Entity> {
	Entity* Value;

	FLuaParam(Entity* val) {
		Value = val;
	}

	void Push(LuaInterface* Lua)
	{
		LuaEntity::PushEntity(Value, Lua);
	}
	void Check(LuaInterface* Lua, int id)
	{
		Value = LuaEntity::CheckEntity(id, Lua);
	}
};
template<>
struct FLuaParam<AUModCharacter> {
	AUModCharacter* Value;

	FLuaParam(AUModCharacter* val) {
		Value = val;
	}

	void Push(LuaInterface* Lua)
	{
		//Lua->PushColor(Value); TODO : add push player method
	}
	void Check(LuaInterface* Lua, int id)
	{
		//Value = Lua->CheckFloat(id); TODO : add check player method
	}
};

enum ETableType {
	GLOBAL,
	GAMEMODE,
	LOCAL //Will believe the table in which to call the function is already on top of the stack at index -1
};

//Macros
#define DECLARE_LUA_FUNC(Name, ...) \
static int LUA_##Name(lua_State *L) { \
	LuaInterface Lua = LuaInterface::Get(L); \
	LUA_AUTOREPLICATE; \

//This macro assumes that the table is at index -1 in the stack
#define LUA_SETTABLE(StrIndex, PushType, Var) \
Lua.PushString(##StrIndex); \
Lua.Push##PushType(##Var); \
Lua.SetTable(-3) \

//Variadic macro only for arguments
#define LUA_TYPEDEF_BEGIN(Name) \
Lua->PushString(#Name); \
Lua->NewMetaTable(#Name) \

#define LUA_TYPEDEF_FUNC(Name, CFunc) \
Lua->PushString(#Name); \
Lua->PushCFunction(LUA_##CFunc); \
Lua->SetTable(-3) \

//Set Entity.__index = Entity (saw in doc, means nothing for me)
//Add Entity metatable to the registry
#define LUA_TYPEDEF_END() \
Lua->PushString("__index"); \
Lua->PushValue(-2); \
Lua->SetTable(-3); \
Lua->SetTable(LUA_REGISTRYINDEX) \

#define LUA_REG_APICLASS(ClassName) \
struct LUA_Initializer##ClassName { \
	LUA_Initializer##ClassName() { \
		LuaEngine::AddClassToRegistry(new ClassName()); \
	} \
}; \
LUA_Initializer##ClassName LUA_Init##ClassName; \

#define LUA_ASSERT_MSG(AssertCode, Msg) \
if (!(##AssertCode)) { \
	Lua.ThrowError(Msg); \
} \

#define LUA_ASSERT(AssertCode) \
if (!(##AssertCode)) { \
	Lua.ThrowError(#AssertCode); \
} \
//End

class UUModGameInstance;

static TArray<LuaLib*> LibRegistry;

/*
The main LuaEngine class for the game
*/
class LuaEngine
{
public:
	LuaEngine(UUModGameInstance *g);
	~LuaEngine();

	bool GameModeInitialized = false;

	void RunScript(FString path);
	FString GetLuaVersion();

	//API to easely implement new libraries in Lua
	void BeginLibReg(FString libName);
	void AddLibFunction(FString name, lua_CFunction func);
	void AddLibConstant(FString name, int i);
	void CreateLibrary();

	static void AddClassToRegistry(LuaLib *Lib);

	//That is failure I tried EVERYTHING Lua seam to crash when calling GetTable inside GetType, Lua IRC refused to help me anyways so I decided to disband the idea of stack trace if anyone knows why it crashes and how to fix it feel free to post a pull request...
	static void HandleLuaError(ELuaErrorType t, FString msg, TArray<FString> &trace);

	void InitGameMode();

	//Running script functions
	bool RunScriptFunctionZeroParam(ETableType Tbl, uint8 resultNumber, FString FuncName);
	template<typename T>
	bool RunScriptFunctionOneParam(ETableType Tbl, uint8 resultNumber, FString FuncName, FLuaParam<T> var0)
	{
		UMOD_STAT(LUAFuncCall1);

		if (!GameModeInitialized) {
			return false;
		}

		Lua->GetGlobal("LUA_InternalErrorHandler");

		int argNum = 1;

		switch (Tbl) {
		case GLOBAL:
			Lua->GetGlobal("_G");
			break;
		case GAMEMODE:
			Lua->GetGlobal("GM");
			argNum++;
			break;
		}
		Lua->PushString(FuncName);
		Lua->GetTable(-2);
		
		if (Tbl == GAMEMODE) {
			Lua->GetGlobal("GM");
		}
		
		var0.Push(Lua);

		ELuaErrorType t = Lua->PCall(argNum, resultNumber, -4);
		if (t != ELuaErrorType::NONE) {
			return false;
		}
		return true;
	}
	template<typename T, typename T1>
	bool RunScriptFunctionTwoParam(ETableType Tbl, uint8 resultNumber, FString FuncName, FLuaParam<T> var0, FLuaParam<T1> var1)
	{
		UMOD_STAT(LUAFuncCall2);

		if (!GameModeInitialized) {
			return false;
		}

		Lua->GetGlobal("LUA_InternalErrorHandler");
		
		int argNum = 2;

		switch (Tbl) {
		case GLOBAL:
			Lua->GetGlobal("_G");
			break;
		case GAMEMODE:
			Lua->GetGlobal("GM");
			argNum++;
			break;
		}
		Lua->PushString(FuncName);
		Lua->GetTable(-2);

		if (Tbl == GAMEMODE) {
			Lua->GetGlobal("GM");
		}

		var0.Push(Lua);
		var1.Push(Lua);
		
		ELuaErrorType t = Lua->PCall(argNum, resultNumber, -5);
		if (t != ELuaErrorType::NONE) {
			return false;
		}
		return true;
	}
	template<typename T, typename T1, typename T2>
	bool RunScriptFunctionThreeParam(ETableType Tbl, uint8 resultNumber, FString FuncName, FLuaParam<T> var0, FLuaParam<T1> var1, FLuaParam<T2> var2)
	{
		UMOD_STAT(LUAFuncCall3);

		if (!GameModeInitialized) {
			return false;
		}

		Lua->GetGlobal("LUA_InternalErrorHandler");

		int argNum = 3;

		switch (Tbl) {
		case GLOBAL:
			Lua->GetGlobal("_G");
			break;
		case GAMEMODE:
			Lua->GetGlobal("GM");
			argNum++;
			break;
		}
		Lua->PushString(FuncName);
		Lua->GetTable(-2);

		if (Tbl == GAMEMODE) {
			Lua->GetGlobal("GM");
		}

		var0.Push(Lua);
		var1.Push(Lua);
		var2.Push(Lua);
		
		ELuaErrorType t = Lua->PCall(argNum, resultNumber, -6);
		if (t != ELuaErrorType::NONE) {
			return false;
		}
		return true;
	}

	//Get the results of functions
	template<typename T>
	void GetScriptFunctionOneResult(FLuaParam<T> &var0)
	{
		var0.Check(Lua, -1);
		//TODO : Pop values
	}
	template<typename T, typename T1>
	void GetScriptFunctionTwoResult(FLuaParam<T> &var0, FLuaParam<T1> &var1)
	{
		var0.Check(Lua, -2);
		var1.Check(Lua, -1);
		//TODO : Pop values
	}
public:
	LuaInterface *Lua;
private:
	FString CurLibName;
};

//Global ErrorHandling func
static int LUA_InternalErrorHandler(lua_State *L)
{
	UMOD_STAT(LUAErrorHandler);

	LuaInterface Lua = LuaInterface::Get(L);
	Lua.TraceBack(1);
	FString ErrorMsg = Lua.ToStringRaw(-5);
	TArray<FString> StackTrace;
	Lua.ToStringRaw(2).ParseIntoArray(StackTrace, TEXT("\n"));
	LuaEngine::HandleLuaError(ELuaErrorType::RUNTIME, ErrorMsg, StackTrace);
	return 0;
}

