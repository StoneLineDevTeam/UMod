// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "LuaInterface.h"
#include <stdarg.h>

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
	//NOTE : Only primitive types and FString are supported
	//'...' Template : type name as character ('d' for double, 'i' for int, 's' for FString, ...)
	void RunScriptFunction(ETableType Tbl, uint8 resultNumber, uint8 ArgNum, FString FuncName, ...);
private:
	LuaInterface *Lua;
	FString CurLibName;
};
