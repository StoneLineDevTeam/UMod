// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "LuaInterface.h"
#include "LuaVector.h"
#include "LuaAngle.h"

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
	FString s;
	switch (GetType(id)) {
	case TABLE:
		s = ToStringRaw(id);
		break;
	case STRING:
		s = ANSI_TO_TCHAR(lua_tostring(luaVM, id));
		break;
	case NUMBER:
		s = ANSI_TO_TCHAR(lua_tostring(luaVM, id));
		break;
	case BOOLS:
	{
		bool b = CheckBool(id);
		if (b) {
			return "TRUE";
		} else {
			return "FALSE";
		}
		break;
	}
	case ENTITY:
		s = "ENTITY[]";
		break;
	case COLOR:
	{
		FColor col = CheckColor(id);
		s = "Color[" + FString::FromInt(col.R) + ";" + FString::FromInt(col.G) + ";" + FString::FromInt(col.B) + ";" + FString::FromInt(col.A) + "]";
		break;
	}
	case VECTOR:		
	{
		FVector vec = CheckVector(id);
		s = "Vector[" + FString::SanitizeFloat(vec.X) + ";" + FString::SanitizeFloat(vec.Y) + ";" + FString::SanitizeFloat(vec.Z) + "]";
		break;
	}
	case ANGLE:
	{
		FRotator rot = CheckAngle(id);
		s = "Angle[" + FString::SanitizeFloat(rot.Pitch) + ";" + FString::SanitizeFloat(rot.Yaw) + ";" + FString::SanitizeFloat(rot.Roll) + "]";
		break;
	}
	case UNKNOWN:
		s = ToStringRaw(id);
		break;
	case FUNCTION:
		s = ToStringRaw(id);
		break;
	case NIL:
		s = "NIL";
		break;
	}
	return s;
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
	case LUA_TFUNCTION:
		return ELuaType::FUNCTION;
	case LUA_TTABLE:
	{		
		PushString("__type");
		if (id < 0) {
			GetTable(id - 1);
		} else {
			GetTable(id + 1);
		}		
		FString type = ANSI_TO_TCHAR(lua_tostring(luaVM, -1));
		Pop(1);
		if (type == "VECTOR") {
			return ELuaType::VECTOR;
		} else if (type == "COLOR") {
			return ELuaType::COLOR;
		} else if (type == "ANGLE") {
			return ELuaType::ANGLE;
		} else if (type == "ENTITY") {
			return ELuaType::ENTITY;
		}
		return ELuaType::TABLE;
	}		
	case LUA_TBOOLEAN:
		return ELuaType::BOOLS;
	case LUA_TSTRING:
		return ELuaType::STRING;
	case LUA_TUSERDATA:
		/*{PushValue(id);
		PushString("__type");
		GetTable(-2);
		FString t = ToString(-1);
		Pop(1);
		if (t == "ENTITY") {
			
		}}*/
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
	if (status != LUA_OK) {
		return ELuaErrorType::PARSER;
	}
	return ELuaErrorType::NONE;
}


//Custom functions that initialy does not exist in Lua
bool LuaInterface::CheckTable(int id)
{
	ELuaType t = GetType(id);
	ArgumentCheck(t == ELuaType::TABLE, id, "expected table");
	if (t == ELuaType::TABLE) {
		return true;
	}
	return false;
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
	ArgumentCheck(GetType(id) == ELuaType::COLOR, id, "expected vector");
	
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

	Pop(1); //This will remove the pushed table from the stack (PushValue(id))

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

void LuaInterface::PushVector(FVector vec)
{
	NewTable();
	PushString("X");
	PushFloat(vec.X);
	SetTable(-3);
	PushString("Y");
	PushFloat(vec.Y);
	SetTable(-3);
	PushString("Z");
	PushFloat(vec.Z);
	SetTable(-3);
	PushString("__type");
	PushString("VECTOR");
	SetTable(-3);
	Vec_AddCFunctions(this);
}

FVector LuaInterface::CheckVector(int id)
{
	ArgumentCheck(GetType(id) == ELuaType::VECTOR, id, "expected vector");
	
	PushValue(id);
	PushString("X");
	GetTable(-2);
	float X = CheckFloat(-1);
	Pop(1); //Removes the float on top
	PushString("Y");
	GetTable(-2);
	float Y = CheckFloat(-1);
	Pop(1); //Removes the float on top
	PushString("Z");
	GetTable(-2);
	float Z = CheckFloat(-1);
	Pop(1); //Removes the float on top

	Pop(1); //This will remove the pushed table from the stack (PushValue(id))

	return FVector(X, Y, Z);
}

void LuaInterface::PushAngle(FRotator rot)
{
	NewTable();
	PushString("Pitch");
	PushFloat(rot.Pitch);
	SetTable(-3);
	PushString("Yaw");
	PushFloat(rot.Yaw);
	SetTable(-3);
	PushString("Roll");
	PushFloat(rot.Roll);
	SetTable(-3);
	PushString("__type");
	PushString("ANGLE");
	SetTable(-3);
	Ang_AddCFunctions(this);
}

FRotator LuaInterface::CheckAngle(int id)
{
	ArgumentCheck(GetType(id) == ELuaType::ANGLE, id, "expected angle");

	PushValue(id);
	PushString("Pitch");
	GetTable(-2);
	float P = CheckFloat(-1);
	Pop(1); //Removes the float on top
	PushString("Yaw");
	GetTable(-2);
	float Y = CheckFloat(-1);
	Pop(1); //Removes the float on top
	PushString("Roll");
	GetTable(-2);
	float R = CheckFloat(-1);
	Pop(1); //Removes the float on top

	Pop(1); //This will remove the pushed table from the stack (PushValue(id))

	return FRotator(P, Y, R);
}

void LuaInterface::ArgumentCheck(bool b, int id, FString msg)
{
	int i = 0;
	if (b) {
		i = 1;
	}	
	luaL_argcheck(luaVM, i, id, TCHAR_TO_ANSI(*msg));
}

bool LuaInterface::IsNil(int id)
{
	if (GetType(id) == NIL) {
		return true;
	}
	return false;
}

int LuaInterface::Ref()
{
	int i =  luaL_ref(luaVM, LUA_REGISTRYINDEX);
	//Now because Lua wants to do in it's head and remove from stack, I say FUCK IT !
	PushRef(i);
	return i;
}
void LuaInterface::PushRef(int ref)
{
	lua_rawgeti(luaVM, LUA_REGISTRYINDEX, ref);
}
void LuaInterface::UnRef(int ref)
{
	luaL_unref(luaVM, LUA_REGISTRYINDEX, ref);
}

void LuaInterface::StackDump(int start, int end)
{
	if (start < 0 && end < 0) {
		for (int i = start; i > end; i--) {
			FString s = ToString(i);
			UE_LOG(UMod_Lua, Warning, TEXT("Level %i: %s"), i, *s);
		}
	} else {
		for (int i = start; i < end; i++) {
			FString s = ToString(i);
			UE_LOG(UMod_Lua, Warning, TEXT("Level %i: %s"), i, *s);
		}
	}	
}

FString LuaInterface::ToStringRaw(int id)
{
	GetGlobal("tostring");
	PushValue(id);
	lua_call(luaVM, 1, 1);
	FString s = ANSI_TO_TCHAR(lua_tostring(luaVM, -1));
	Pop(2);
	return s;
}