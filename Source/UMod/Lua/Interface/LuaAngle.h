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
	Lua.PushValue(1);
	Lua.PushString("Pitch");
	Lua.PushFloat(result.Pitch);
	Lua.SetTable(-3);
	Lua.PushString("Yaw");
	Lua.PushFloat(result.Yaw);
	Lua.SetTable(-3);
	Lua.PushString("Roll");
	Lua.PushFloat(result.Roll);
	Lua.SetTable(-3);
	return 0;
}

static int Ang_IsZero(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FRotator v = Lua.CheckAngle(1); //Get the "self" vector
	if (!Lua.IsNil(2)) {
		float f = Lua.CheckFloat(2);
		Lua.PushBool(v.IsNearlyZero(f));
	} else {
		Lua.PushBool(v.IsZero());
	}
	return 1;
}

static int Ang_ContainsNaN(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FRotator r = Lua.CheckAngle(1);
	Lua.PushBool(r.ContainsNaN());
	return 1;
}

static int Ang_Equals(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FRotator r = Lua.CheckAngle(1);
	FRotator r1 = Lua.CheckAngle(2);
	float t = Lua.CheckFloat(3);
	Lua.PushBool(r.Equals(r1, t));	
	return 1;
}

static int Ang_Denormalize(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FRotator r = Lua.CheckAngle(1);
	FRotator r1 = r.GetDenormalized();
	Lua.PushValue(1);
	Lua.PushString("Pitch");
	Lua.PushFloat(r1.Pitch);
	Lua.SetTable(-3);
	Lua.PushString("Yaw");
	Lua.PushFloat(r1.Yaw);
	Lua.SetTable(-3);
	Lua.PushString("Roll");
	Lua.PushFloat(r1.Roll);
	Lua.SetTable(-3);
	return 0;
}

static int Ang_Inverse(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FRotator r = Lua.CheckAngle(1);
	FRotator r1 = r.GetInverse();
	Lua.PushValue(1);
	Lua.PushString("Pitch");
	Lua.PushFloat(r1.Pitch);
	Lua.SetTable(-3);
	Lua.PushString("Yaw");
	Lua.PushFloat(r1.Yaw);
	Lua.SetTable(-3);
	Lua.PushString("Roll");
	Lua.PushFloat(r1.Roll);
	Lua.SetTable(-3);
	return 0;
}

static int Ang_Normalize(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FRotator r = Lua.CheckAngle(1);
	FRotator r1 = r.GetNormalized();
	Lua.PushValue(1);
	Lua.PushString("Pitch");
	Lua.PushFloat(r1.Pitch);
	Lua.SetTable(-3);
	Lua.PushString("Yaw");
	Lua.PushFloat(r1.Yaw);
	Lua.SetTable(-3);
	Lua.PushString("Roll");
	Lua.PushFloat(r1.Roll);
	Lua.SetTable(-3);
	return 0;
}

static int Ang_GridSnap(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FRotator r = Lua.CheckAngle(1);
	FRotator r1 = Lua.CheckAngle(2);
	Lua.PushAngle(r.GridSnap(r1));
	return 1;
}

static int Ang_Add(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FRotator r = Lua.CheckAngle(1);
	float deltaP = Lua.CheckFloat(2);
	float deltaY = Lua.CheckFloat(3);
	float deltaR = Lua.CheckFloat(4);
	FRotator r1 = r.Add(deltaP, deltaY, deltaR);
	Lua.PushValue(1);
	Lua.PushString("Pitch");
	Lua.PushFloat(r1.Pitch);
	Lua.SetTable(-3);
	Lua.PushString("Yaw");
	Lua.PushFloat(r1.Yaw);
	Lua.SetTable(-3);
	Lua.PushString("Roll");
	Lua.PushFloat(r1.Roll);
	Lua.SetTable(-3);	
	return 0;
}

static void Ang_AddCFunctions(LuaInterface *Lua) {
	Lua->PushString("RotateArroundAxis");
	Lua->PushCFunction(Ang_RotateArroundAxis);
	Lua->SetTable(-3);
	Lua->PushString("Up");
	Lua->PushCFunction(Ang_Up);
	Lua->SetTable(-3);
	Lua->PushString("Forward");
	Lua->PushCFunction(Ang_Forward);
	Lua->SetTable(-3);
	Lua->PushString("Left");
	Lua->PushCFunction(Ang_Left);
	Lua->SetTable(-3);
	Lua->PushString("Clamp");
	Lua->PushCFunction(Ang_Clamp);
	Lua->SetTable(-3);
	Lua->PushString("IsZero");
	Lua->PushCFunction(Ang_IsZero);
	Lua->SetTable(-3);
	Lua->PushString("ContainsNaN");
	Lua->PushCFunction(Ang_ContainsNaN);
	Lua->SetTable(-3);
	Lua->PushString("Equals");
	Lua->PushCFunction(Ang_Equals);
	Lua->SetTable(-3);
	Lua->PushString("Denormalize");
	Lua->PushCFunction(Ang_Denormalize);
	Lua->SetTable(-3);
	Lua->PushString("Normalize");
	Lua->PushCFunction(Ang_Normalize);
	Lua->SetTable(-3);
	Lua->PushString("Inverse");
	Lua->PushCFunction(Ang_Inverse);
	Lua->SetTable(-3);
	Lua->PushString("GridSnap");
	Lua->PushCFunction(Ang_GridSnap);
	Lua->SetTable(-3);
	Lua->PushString("Add");
	Lua->PushCFunction(Ang_Add);
	Lua->SetTable(-3);
}