// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "Interface/LuaInterface.h"
#include "LuaEntity.h"

class AUModCharacter;
class AEntityBase;
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
struct FLuaParam<AEntityBase> {
	AEntityBase* Value;

	FLuaParam(AEntityBase* val) {
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
#define DECLARE_LUA_FUNC(Name) \
static int LUA_##Name(lua_State *L) { \
	LuaInterface Lua = LuaInterface::Get(L); \

#define DECLARE_LUA_FUNC_OneParam(Name, Interface, TypeName, VarName) \
static int LUA_##Name(lua_State *L) { \
	LuaInterface Lua = LuaInterface::Get(L); \
	##TypeName ##VarName = Lua.Check##Interface(); \

#define DECLARE_LUA_FUNC_TwoParam(Name, Interface, TypeName, VarName, Interface1, TypeName1, VarName1) \
static int LUA_##Name(lua_State *L) { \
	LuaInterface Lua = LuaInterface::Get(L); \
	##TypeName ##VarName = Lua.Check##Interface(); \
	##TypeName1 ##VarName1 = Lua.Check##Interface1(); \

#define DECLARE_LUA_FUNC_ThreeParam(Name, Interface, TypeName, VarName, Interface1, TypeName1, VarName1, Interface2, TypeName2, VarName2) \
static int LUA_##Name(lua_State *L) { \
	LuaInterface Lua = LuaInterface::Get(L); \
	##TypeName ##VarName = Lua.Check##Interface(); \
	##TypeName1 ##VarName1 = Lua.Check##Interface1(); \
	##TypeName2 ##VarName2 = Lua.Check##Interface2(); \

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
//End

class UUModGameInstance;

/*
The main LuaEngine class for the game
*/
class LuaEngine
{
public:
	LuaEngine(UUModGameInstance *g);
	~LuaEngine();

	void RunScript(FString path);
	FString GetLuaVersion();

	//API to easely implement new libraries in Lua
	void BeginLibReg(FString libName);
	void AddLibFunction(FString name, lua_CFunction func);
	void AddLibConstant(FString name, int i);
	void CreateLibrary();
	
	void HandleLuaError(ELuaErrorType t, FString msg);

	//Running script functions
	void RunScriptFunctionZeroParam(ETableType Tbl, uint8 resultNumber, FString FuncName);
	template<typename T>
	void RunScriptFunctionOneParam(ETableType Tbl, uint8 resultNumber, FString FuncName, FLuaParam<T> var0)
	{
		//Lua->TraceBack(-1);
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

		ELuaErrorType t = Lua->PCall(argNum, resultNumber, 0);
		if (t != ELuaErrorType::NONE) {
			FString msg = Lua->ToString(-1);
			HandleLuaError(t, msg);
		}		
	}
	template<typename T, typename T1>
	void RunScriptFunctionTwoParam(ETableType Tbl, uint8 resultNumber, FString FuncName, FLuaParam<T> var0, FLuaParam<T1> var1)
	{
		//Lua->TraceBack(-1);
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

		ELuaErrorType t = Lua->PCall(argNum, resultNumber, 0);
		if (t != ELuaErrorType::NONE) {
			FString msg = Lua->ToString(-1);
			HandleLuaError(t, msg);
		}
	}
	template<typename T, typename T1, typename T2>
	void RunScriptFunctionThreeParam(ETableType Tbl, uint8 resultNumber, FString FuncName, FLuaParam<T> var0, FLuaParam<T1> var1, FLuaParam<T2> var2)
	{
		//Lua->TraceBack(-1);
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

		ELuaErrorType t = Lua->PCall(argNum, resultNumber, 0);
		if (t != ELuaErrorType::NONE) {
			FString msg = Lua->ToString(-1);
			HandleLuaError(t, msg);
		}
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
