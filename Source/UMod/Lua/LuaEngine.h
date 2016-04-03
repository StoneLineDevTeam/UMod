// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "LuaInterface.h"

class AUModCharacter;
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
		UE_LOG(UMod_Lua, Warning, TEXT("[DEBUG]Lua->PushString(%s)"), *Value);
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

class UUModGameInstance;

/*
The main LuaEngine class for the game
*/
class UMOD_API LuaEngine
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

		var0.Push(Lua);

		ELuaErrorType t = Lua->PCall(1, resultNumber, 1);
		if (t != ELuaErrorType::NONE) {
			FString msg = Lua->ToString(-1);
			HandleLuaError(t, msg);
		}
	}
	template<typename T, typename T1>
	void RunScriptFunctionTwoParam(ETableType Tbl, uint8 resultNumber, FString FuncName, FLuaParam<T> var0, FLuaParam<T1> var1)
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

		var0.Push(Lua);
		var1.Push(Lua);

		ELuaErrorType t = Lua->PCall(2, resultNumber, 1);
		if (t != ELuaErrorType::NONE) {
			FString msg = Lua->ToString(-1);
			HandleLuaError(t, msg);
		}
	}
	template<typename T, typename T1, typename T2>
	void RunScriptFunctionThreeParam(ETableType Tbl, uint8 resultNumber, FString FuncName, FLuaParam<T> var0, FLuaParam<T1> var1, FLuaParam<T2> var2)
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

		var0.Push(Lua);
		var1.Push(Lua);
		var2.Push(Lua);

		ELuaErrorType t = Lua->PCall(3, resultNumber, 1);
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
