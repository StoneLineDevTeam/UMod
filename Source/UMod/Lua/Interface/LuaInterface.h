// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

extern "C"
{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
}

//class UUModGameInstance;

enum ELuaErrorType {
	PARSER,
	RUNTIME,
	MEMORY,
	HANDLER,
	NONE
};

enum ELuaType {
	TABLE,
	STRING,
	NUMBER,
	BOOLS,
	ENTITY,
	COLOR,
	VECTOR,
	ANGLE,
	UNKNOWN,
	FUNCTION,
	NIL
};

/**
 * The Lua Interface used as bridge between UE4C++ and Lua
 * As you can see I thought uisng a basic C++ class would be better to assure no memory leaks (UE4 uses it's own Garbage Collector, so...).
 * ----------------------------------------------------------------------------------------------------------------------------------------
 * Needs the lua dlls to be inside the bin dir
 */
class LuaInterface
{
public:
	static LuaInterface* New();
	static LuaInterface Get(lua_State *L);

	void PushString(FString str);
	void PushInt(int i);	
	void PushNum(double d);	
	void PushFloat(float f);
	void PushBool(bool b);
	void PushCFunction(lua_CFunction f);
	void PushNil();
	//Copies value at id and push on top of stack
	void PushValue(int id);
	//Future methods that requires further understanding of lua tables
	void PushColor(FColor col);
	void PushVector(FVector vec);
	void PushAngle(FRotator rot);

	void SetGlobal(FString str);
	void NewTable();
	void NewMetaTable(FString str);
	//Takes table index and reads key at slot id - 1
	void GetTable(int id);
	//Takes table index and reads key at slot id - 1 sets value at slot id - 2
	void SetTable(int id);
	void GetMetaTable(int id);
	void SetMetaTable(int id);
	void GetGlobal(FString str);
	ELuaType GetType(int id);
	void OpenLibs();
	ELuaErrorType LoadFile(FString file);

	//Used to iterate lua tables
	int Next(int id);
	void Pop(int id);
	//End

	void ArgumentCheck(bool b, int id, FString msg); //Something I wanted to do before

	//UserData
	void* CheckUserData(int id, FString str);
	void* NewUserData(int size);
	//End

	void TraceBack(int Level);

	void ThrowError(FString msg);
	ELuaErrorType PCall(int argNum, int resultNum, int handler);

	FString CheckString(int id);
	FString ToString(int id);
	int CheckInt(int id);
	double CheckNum(int id);
	float CheckFloat(int id);
	bool CheckBool(int id);
	bool CheckTable(int id);
	//Future methods that requires further understanding of lua tables
	FColor CheckColor(int id);
	FVector CheckVector(int id);
	FRotator CheckAngle(int id);

	bool IsNil(int id);

	/*Refs*/
	int Ref(); //Stores a reference for the value at level -1
	void PushRef(int ref); //Pushes the given reference onto the stack
	void UnRef(int ref);
	/*End*/

	void Register(FString name, lua_CFunction func);

	void StackDump(int start, int end); //Warning : inputs must be in negative order (ex : dump from level -1 to level -10)

	FString GetVersion();

	FString ToStringRaw(int id);

	void Close();
private:
	lua_State *luaVM;
};
