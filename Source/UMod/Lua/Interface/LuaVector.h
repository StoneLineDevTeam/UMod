#pragma once
#include "LuaInterface.h"
#include "UMod.h"

#define LUA_AUTOREPLICATE

DECLARE_LUA_FUNC(Vector_Length)
	FVector v = Lua.CheckVector(1); //Get the "self" vector
	Lua.PushFloat(v.Size());
	return 1;
}

DECLARE_LUA_FUNC(Vector_Distance)	
	FVector v = Lua.CheckVector(1); //Get the "self" vector
	FVector v1 = Lua.CheckVector(2); //Get the param vector
	Lua.PushFloat(FVector::Dist(v, v1));
	return 1;
}

DECLARE_LUA_FUNC(Vector_ContainsNaN)	
	FVector v = Lua.CheckVector(1); //Get the "self" vector	
	Lua.PushBool(v.ContainsNaN());
	return 1;
}

DECLARE_LUA_FUNC(Vector_IsZero, (Float))	
	FVector v = Lua.CheckVector(1); //Get the "self" vector
	if (!Lua.IsNil(2)) {
		float f = Lua.CheckFloat(2);
		Lua.PushBool(v.IsNearlyZero(f));
	} else {
		Lua.PushBool(v.IsZero());
	}
	return 1;
}

DECLARE_LUA_FUNC(Vector_Cross, Vector)	
	FVector v = Lua.CheckVector(1); //Get the "self" vector	
	FVector v1 = Lua.CheckVector(2);
	Lua.PushVector(FVector::CrossProduct(v, v1));
	return 1;
}

DECLARE_LUA_FUNC(Vector_Dot, Vector)	
	FVector v = Lua.CheckVector(1); //Get the "self" vector	
	FVector v1 = Lua.CheckVector(2);
	Lua.PushFloat(FVector::DotProduct(v, v1));
	return 1;
}

DECLARE_LUA_FUNC(Vector_Equals, Vector, Float)	
	FVector v = Lua.CheckVector(1); //Get the "self" vector	
	FVector v1 = Lua.CheckVector(2);
	float t = Lua.CheckFloat(3);
	Lua.PushBool(v.Equals(v1, t));
	return 1;
}

DECLARE_LUA_FUNC(Vector_GridSnap, Float)	
	FVector v = Lua.CheckVector(1);	//Get the "self" vector	
	float t = Lua.CheckFloat(2);
	Lua.PushVector(v.GridSnap(t));
	return 1;
}

DECLARE_LUA_FUNC(Vector_ToScreen)	
	FVector v = Lua.CheckVector(1);	//Get the "self" vector	
	Lua.PushVector(v.Projection());
	return 1;
}

DECLARE_LUA_FUNC(Vector_Normalize, Float)	
	FVector v = Lua.CheckVector(1); //Get the "self" vector	
	float f = Lua.CheckFloat(2);
	v.Normalize(f);
	Lua.PushValue(1);
	LUA_SETTABLE("X", Float, v.X);
	LUA_SETTABLE("Y", Float, v.Y);
	LUA_SETTABLE("Z", Float, v.Z);
	return 0;
}

//Lua does not support operator-
DECLARE_LUA_FUNC(Vector_Subtract, Vector)	
	FVector v = Lua.CheckVector(1); //Get the "self" vector
	FVector v1 = Lua.CheckVector(2); //Get the param vector
	FVector v2 = v - v1;
	Lua.PushValue(1);
	LUA_SETTABLE("X", Float, v2.X);
	LUA_SETTABLE("Y", Float, v2.Y);
	LUA_SETTABLE("Z", Float, v2.Z);
	return 0;
}

//Lua does not support operator*
DECLARE_LUA_FUNC(Vector_Multiply, Vector or Float)	
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
	LUA_SETTABLE("X", Float, v2.X);
	LUA_SETTABLE("Y", Float, v2.Y);
	LUA_SETTABLE("Z", Float, v2.Z);
	return 0;
}

//Lua does not support operator+
DECLARE_LUA_FUNC(Vector_Add, Vector)
	FVector v = Lua.CheckVector(1); //Get the "self" vector
	FVector v1 = Lua.CheckVector(2); //Get the param vector
	FVector v2 = v + v1;
	Lua.PushValue(1);
	LUA_SETTABLE("X", Float, v2.X);
	LUA_SETTABLE("Y", Float, v2.Y);
	LUA_SETTABLE("Z", Float, v2.Z);
	return 0;
}

#undef LUA_AUTOREPLICATE

static void Vec_AddCFunctions(LuaInterface *Lua) {
	LUA_TYPEDEF_FUNC(Length, Vector_Length);
	LUA_TYPEDEF_FUNC(Distance, Vector_Distance);
	LUA_TYPEDEF_FUNC(ContainsNaN, Vector_ContainsNaN);
	LUA_TYPEDEF_FUNC(IsZero, Vector_IsZero);
	LUA_TYPEDEF_FUNC(Cross, Vector_Cross);
	LUA_TYPEDEF_FUNC(Dot, Vector_Dot);
	LUA_TYPEDEF_FUNC(Equals, Vector_Equals);
	LUA_TYPEDEF_FUNC(GridSnap, Vector_GridSnap);
	LUA_TYPEDEF_FUNC(ToScreen, Vector_ToScreen);
	LUA_TYPEDEF_FUNC(Normalize, Vector_Normalize);
	LUA_TYPEDEF_FUNC(Add, Vector_Add);
	LUA_TYPEDEF_FUNC(Subtract, Vector_Subtract);
	LUA_TYPEDEF_FUNC(Multiply, Vector_Multiply);
}