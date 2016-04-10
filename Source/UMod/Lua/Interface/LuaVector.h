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

static int Vec_ContainsNaN(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FVector v = Lua.CheckVector(1); //Get the "self" vector	
	Lua.PushBool(v.ContainsNaN());
	return 1;
}

static int Vec_IsZero(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FVector v = Lua.CheckVector(1); //Get the "self" vector
	if (!Lua.IsNil(2)) {
		float f = Lua.CheckFloat(2);
		Lua.PushBool(v.IsNearlyZero(f));
	} else {
		Lua.PushBool(v.IsZero());
	}
	return 1;
}

static int Vec_Cross(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FVector v = Lua.CheckVector(1); //Get the "self" vector	
	FVector v1 = Lua.CheckVector(2);
	Lua.PushVector(FVector::CrossProduct(v, v1));
	return 1;
}

static int Vec_Dot(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FVector v = Lua.CheckVector(1); //Get the "self" vector	
	FVector v1 = Lua.CheckVector(2);
	Lua.PushFloat(FVector::DotProduct(v, v1));
	return 1;
}

static int Vec_Equals(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FVector v = Lua.CheckVector(1);
	FVector v1 = Lua.CheckVector(2);
	float t = Lua.CheckFloat(3);
	Lua.PushBool(v.Equals(v1, t));
	return 1;
}

static int Vec_GridSnap(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FVector v = Lua.CheckVector(1);	
	float t = Lua.CheckFloat(2);
	Lua.PushVector(v.GridSnap(t));
	return 1;
}

static int Vec_ToScreen(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FVector v = Lua.CheckVector(1);	
	Lua.PushVector(v.Projection());
	return 1;
}

static int Vec_Normalize(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FVector v = Lua.CheckVector(1); //Get the "self" vector	
	float f = Lua.CheckFloat(2);
	v.Normalize(f);
	Lua.PushValue(1);
	Lua.PushString("X");
	Lua.PushFloat(v.X);
	Lua.SetTable(-3);
	Lua.PushString("Y");
	Lua.PushFloat(v.Y);
	Lua.SetTable(-3);
	Lua.PushString("Z");
	Lua.PushFloat(v.Z);
	Lua.SetTable(-3);
	return 0;
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
	ELuaType t = Lua.GetType(2);
	FVector v2;
	if (t == NUMBER) {
		FVector v1 = Lua.CheckVector(2); //Get the param vector
		v2 = v * v1;
	} else if (t == VECTOR) {
		float f = Lua.CheckFloat(2);
		v2 = v * f;
	}	
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
	Lua->PushString("ContainsNaN");
	Lua->PushCFunction(Vec_ContainsNaN);
	Lua->SetTable(-3);
	Lua->PushString("IsZero");
	Lua->PushCFunction(Vec_IsZero);
	Lua->SetTable(-3);
	Lua->PushString("Cross");
	Lua->PushCFunction(Vec_Cross);
	Lua->SetTable(-3);
	Lua->PushString("Dot");
	Lua->PushCFunction(Vec_Dot);
	Lua->SetTable(-3);
	Lua->PushString("Equals");
	Lua->PushCFunction(Vec_Equals);
	Lua->SetTable(-3);
	Lua->PushString("GridSnap");
	Lua->PushCFunction(Vec_GridSnap);
	Lua->SetTable(-3);
	Lua->PushString("ToScreen");
	Lua->PushCFunction(Vec_ToScreen);
	Lua->SetTable(-3);
	Lua->PushString("Normalize");
	Lua->PushCFunction(Vec_Normalize);
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