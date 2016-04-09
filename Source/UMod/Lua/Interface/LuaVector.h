#pragma once
#include "LuaInterface.h"
#include "UMod.h"

static int Vec_Length(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FVector v = Lua.CheckVector(1);
	Lua.PushFloat(v.Size());
	return 1;
}

static int Vec_Distance(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FVector v = Lua.CheckVector(1); //Get the "self" vector
	FVector v1 = Lua.CheckVector(2); //Get the param vector
	Lua.PushFloat(FVector::Dist(v, v1));
	return 1;
}

//Lua does not support operator-
static int Vec_Subtract(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FVector v = Lua.CheckVector(1); //Get the "self" vector
	FVector v1 = Lua.CheckVector(2); //Get the param vector
	FVector v2 = v - v1;
	Lua.PushValue(1);
	Lua.PushString("X");
	Lua.PushFloat(v2.X);
	Lua.SetTable(-3);
	Lua.PushString("Y");
	Lua.PushFloat(v2.Y);
	Lua.SetTable(-3);
	Lua.PushString("Z");
	Lua.PushFloat(v2.Z);
	Lua.SetTable(-3);
	return 0;
}

//Lua does not support operator*
static int Vec_Multiply(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FVector v = Lua.CheckVector(1); //Get the "self" vector
	FVector v1 = Lua.CheckVector(2); //Get the param vector
	FVector v2 = v * v1;
	Lua.PushValue(1);
	Lua.PushString("X");
	Lua.PushFloat(v2.X);
	Lua.SetTable(-3);
	Lua.PushString("Y");
	Lua.PushFloat(v2.Y);
	Lua.SetTable(-3);
	Lua.PushString("Z");
	Lua.PushFloat(v2.Z);
	Lua.SetTable(-3);

	return 0;
}

//Lua does not support operator+
static int Vec_Add(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FVector v = Lua.CheckVector(1); //Get the "self" vector
	FVector v1 = Lua.CheckVector(2); //Get the param vector
	FVector v2 = v + v1;
	Lua.PushValue(1);
	Lua.PushString("X");
	Lua.PushFloat(v2.X);
	Lua.SetTable(-3);
	Lua.PushString("Y");
	Lua.PushFloat(v2.Y);
	Lua.SetTable(-3);
	Lua.PushString("Z");
	Lua.PushFloat(v2.Z);
	Lua.SetTable(-3);

	return 0;
}

static void Vec_AddCFunctions(LuaInterface *Lua) {
	Lua->PushString("Length");
	Lua->PushCFunction(Vec_Length);
	Lua->SetTable(-3);
	Lua->PushString("Distance");
	Lua->PushCFunction(Vec_Distance);
	Lua->SetTable(-3);
	Lua->PushString("Add");
	Lua->PushCFunction(Vec_Add);
	Lua->SetTable(-3);
	Lua->PushString("Subtract");
	Lua->PushCFunction(Vec_Subtract);
	Lua->SetTable(-3);
	Lua->PushString("Multiply");
	Lua->PushCFunction(Vec_Multiply);
	Lua->SetTable(-3);
}