#pragma once
#include "LuaInterface.h"
#include "UMod.h"

static int Ang_RotateArroundAxis(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FRotator r = Lua.CheckAngle(1); //The "self" angle
	FVector axis = Lua.CheckVector(2);
	float rot = Lua.CheckFloat(3);
	FVector v = r.Vector().RotateAngleAxis(rot, axis);
	Lua.PushValue(1);
	Lua.PushString("Pitch");
	Lua.PushFloat(v.Rotation().Pitch);
	Lua.SetTable(-3);
	Lua.PushString("Yaw");
	Lua.PushFloat(v.Rotation().Yaw);
	Lua.SetTable(-3);
	Lua.PushString("Roll");
	Lua.PushFloat(v.Rotation().Roll);
	Lua.SetTable(-3);

	return 0;
}

static int Ang_Up(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FRotator r = Lua.CheckAngle(1);
	//Up vector is BP only, I'll find a hack later

	return 1;
}

static int Ang_Forward(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FRotator r = Lua.CheckAngle(1);
	//Forward vector is BP only, I'll find a hack later

	return 1;
}

static int Ang_Left(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FRotator r = Lua.CheckAngle(1);
	//Left vector is BP only, I'll find a hack later

	return 1;
}

static int Ang_Clamp(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FRotator r = Lua.CheckAngle(1);
	FRotator result = r.Clamp();
	Lua.PushAngle(result);

	return 1;
}

static int Ang_Add(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	return 0;
}

static void Ang_AddCFunctions(LuaInterface *Lua) {

}