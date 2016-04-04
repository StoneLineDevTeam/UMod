// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "LuaInterface.h"

LuaInterface* LuaInterface::New()
{
	LuaInterface *Lua = new LuaInterface();
	Lua->luaVM = luaL_newstate();
	if (Lua->luaVM == NULL) {
		return NULL;
	}
	return Lua;
}

LuaInterface LuaInterface::Get(lua_State *L)
{
	LuaInterface Lua;
	Lua.luaVM = L;
	return Lua;
}

void LuaInterface::Close()
{
	lua_close(luaVM);
	delete this;
}

void LuaInterface::PushString(FString str)
{
	lua_pushstring(luaVM, TCHAR_TO_ANSI(*str));
}

void LuaInterface::PushInt(int i)
{
	lua_pushinteger(luaVM, i);
}

void LuaInterface::PushNum(double d)
{
	lua_pushnumber(luaVM, d);
}

void LuaInterface::PushCFunction(lua_CFunction f)
{
	lua_pushcfunction(luaVM, f);
}

void LuaInterface::PushNil()
{
	lua_pushnil(luaVM);
}

void LuaInterface::PushValue(int id)
{
	lua_pushvalue(luaVM, id);
}

void LuaInterface::SetGlobal(FString name)
{
	lua_setglobal(luaVM, TCHAR_TO_ANSI(*name));
}

void LuaInterface::PushBool(bool b)
{
	if (b) {
		lua_pushboolean(luaVM, 1);
	} else {
		lua_pushboolean(luaVM, 0);
	}	
}

void LuaInterface::NewTable()
{
	lua_newtable(luaVM);
}

void LuaInterface::GetTable(int id)
{
	lua_gettable(luaVM, id);
}

void LuaInterface::GetMetaTable(int id)
{
	lua_getmetatable(luaVM, id);
}

void LuaInterface::SetTable(int id)
{
	lua_settable(luaVM, id);
}

void LuaInterface::SetMetaTable(int id)
{
	lua_setmetatable(luaVM, id);
}


FString LuaInterface::CheckString(int id)
{
	TCHAR* s = ANSI_TO_TCHAR(luaL_checkstring(luaVM, id));
	return FString(s);
}

FString LuaInterface::ToString(int id)
{
	TCHAR* s = ANSI_TO_TCHAR(lua_tostring(luaVM, id));
	return FString(s);
}

int LuaInterface::CheckInt(int id)
{
	return luaL_checkinteger(luaVM, id);
}

double LuaInterface::CheckNum(int id)
{
	return luaL_checknumber(luaVM, id);
}

bool LuaInterface::CheckBool(int id)
{
	int i = lua_toboolean(luaVM, id);
	return i == 1;
}

FString LuaInterface::GetVersion()
{
	const lua_Number* n = lua_version(luaVM);
	double d = *n;
	return FString::SanitizeFloat(d);
}

void LuaInterface::GetGlobal(FString str)
{
	lua_getglobal(luaVM, TCHAR_TO_ANSI(*str));
}

void LuaInterface::ThrowError(FString msg)
{
	PushString(msg);
	lua_error(luaVM);
}

ELuaErrorType LuaInterface::PCall(int argNum, int resultNum, int handler)
{
	int err = lua_pcall(luaVM, argNum, resultNum, handler);
	switch (err)
	{
	case LUA_ERRRUN:
		return ELuaErrorType::RUNTIME;
	case LUA_ERRMEM:
		return ELuaErrorType::MEMORY;
	case LUA_ERRERR:
		return ELuaErrorType::HANDLER;
	}
	return ELuaErrorType::NONE;
}

ELuaType LuaInterface::GetType(int id)
{
	int i = lua_type(luaVM, id);
	switch (i) {
	case LUA_TNUMBER:
		return ELuaType::NUMBER;
	case LUA_TTABLE:
		{PushValue(id);
		PushString("__type");
		GetTable(-3);
		FString type = ToString(-1);
		Pop(1);
		if (type == "COLOR") {
			return ELuaType::COLOR;
		}}
		return ELuaType::TABLE;
	case LUA_TBOOLEAN:
		return ELuaType::BOOLS;
	case LUA_TSTRING:
		return ELuaType::STRING;
	case LUA_TUSERDATA:
		{PushValue(id);
		PushString("__type");
		GetTable(-3);
		FString t = ToString(-1);
		Pop(1);
		if (t == "VECTOR") {
			return ELuaType::VECTOR;
		} else if (t == "ENTITY") {
			return ELuaType::ENTITY;
		}}
		return ELuaType::UNKNOWN;
	}
	return ELuaType::NIL;
}

void LuaInterface::OpenLibs()
{
	luaL_openlibs(luaVM);
}

ELuaErrorType LuaInterface::LoadFile(FString file)
{
	int status = luaL_loadfile(luaVM, TCHAR_TO_ANSI(*file));
	if (status == 1) {
		ELuaErrorType::PARSER;
	}
	return ELuaErrorType::NONE;
}


//Custom functions that initialy does not exist in Lua
bool LuaInterface::CheckTable(int id)
{
	ELuaType t = GetType(id);
	if (t == ELuaType::TABLE) {
		return true;
	}
	else {
		ThrowError("Expected table, got " + (int)GetType(id));
		return false;
	}
}

void LuaInterface::PushColor(FColor col)
{
	uint8 r = col.R;
	uint8 g = col.G;
	uint8 b = col.B;
	uint8 a = col.A;
	NewTable();
	PushString("R");
	PushInt((int)r);
	SetTable(-3);
	PushString("G");
	PushInt((int)g);
	SetTable(-3);
	PushString("B");
	PushInt((int)b);
	SetTable(-3);
	PushString("A");
	PushInt((int)a);
	SetTable(-3);
	PushString("__type");
	PushString("COLOR");
	SetTable(-3);
}

FColor LuaInterface::CheckColor(int id)
{
	if (GetType(id) != ELuaType::COLOR) {
		ThrowError("Expected color got : " + (int)GetType(id));
		return FColor();
	}
	
	PushValue(id);
	
	PushString("R");
	GetTable(-2);
	uint8 r = (uint8)CheckInt(-1);
	Pop(1);
	
	PushString("G");
	GetTable(-2);
	uint8 g = (uint8)CheckInt(-1);
	Pop(1);

	PushString("B");
	GetTable(-2);
	uint8 b = (uint8)CheckInt(-1);
	Pop(1);

	PushString("A");
	GetTable(-2);
	uint8 a = (uint8)CheckInt(-1);
	Pop(1);

	return FColor(r, g, b, a);
}

float LuaInterface::CheckFloat(int id)
{
	return (float)CheckNum(id);
}

int LuaInterface::Next(int id)
{
	return lua_next(luaVM, id);
}

void LuaInterface::Pop(int id)
{
	lua_pop(luaVM, id);
}

void LuaInterface::PushFloat(float f)
{
	PushNum((double)f);
}

void LuaInterface::TraceBack(int Level)
{
	luaL_traceback(luaVM, luaVM, NULL, Level);
}

void* LuaInterface::NewUserData(int size)
{
	return lua_newuserdata(luaVM, size);
}

void* LuaInterface::CheckUserData(int id, FString str)
{
	return luaL_checkudata(luaVM, id, TCHAR_TO_ANSI(*str));
}

void LuaInterface::NewMetaTable(FString str)
{
	luaL_newmetatable(luaVM, TCHAR_TO_ANSI(*str));
}