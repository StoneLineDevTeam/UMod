#pragma once
#include "LuaInterface.h"
#include "UMod.h"

//The "self" angle
#define LUA_AUTOREPLICATE FRotator self = Lua.CheckAngle(1);

DECLARE_LUA_FUNC(Angle_RotateArroundAxis, Vector, Float)
	FVector axis = Lua.CheckVector(2);
	float rot = Lua.CheckFloat(3);
	FVector v = self.Vector().RotateAngleAxis(rot, axis);
	Lua.PushValue(1);
	LUA_SETTABLE("Pitch", Float, v.Rotation().Pitch);
	LUA_SETTABLE("Yaw", Float, v.Rotation().Yaw);
	LUA_SETTABLE("Roll", Float, v.Rotation().Roll);
	return 0;
}

DECLARE_LUA_FUNC(Angle_Up)
	//Up vector is BP only, I'll find a hack later

	return 1;
}

DECLARE_LUA_FUNC(Angle_Forward)
	//Forward vector is BP only, I'll find a hack later

	return 1;
}

DECLARE_LUA_FUNC(Angle_Left)
	//Left vector is BP only, I'll find a hack later

	return 1;
}

DECLARE_LUA_FUNC(Angle_Clamp)
	FRotator result = self.Clamp();
	Lua.PushValue(1);
	LUA_SETTABLE("Pitch", Float, result.Pitch);
	LUA_SETTABLE("Yaw", Float, result.Yaw);
	LUA_SETTABLE("Roll", Float, result.Roll);
	return 0;
}

DECLARE_LUA_FUNC(Angle_IsZero, (Float))
	if (!Lua.IsNil(2)) {
		float f = Lua.CheckFloat(2);
		Lua.PushBool(self.IsNearlyZero(f));
	} else {
		Lua.PushBool(self.IsZero());
	}
	return 1;
}

DECLARE_LUA_FUNC(Angle_ContainsNaN)
	Lua.PushBool(self.ContainsNaN());
	return 1;
}

DECLARE_LUA_FUNC(Angle_Equals, Angle, Float)
	FRotator r1 = Lua.CheckAngle(2);
	float t = Lua.CheckFloat(3);
	Lua.PushBool(self.Equals(r1, t));
	return 1;
}

DECLARE_LUA_FUNC(Angle_Denormalize)
	FRotator r1 = self.GetDenormalized();
	Lua.PushValue(1);
	LUA_SETTABLE("Pitch", Float, r1.Pitch);
	LUA_SETTABLE("Yaw", Float, r1.Yaw);
	LUA_SETTABLE("Roll", Float, r1.Roll);
	return 0;
}

DECLARE_LUA_FUNC(Angle_Inverse)	
	FRotator r1 = self.GetInverse();
	Lua.PushValue(1);
	LUA_SETTABLE("Pitch", Float, r1.Pitch);
	LUA_SETTABLE("Yaw", Float, r1.Yaw);
	LUA_SETTABLE("Roll", Float, r1.Roll);
	return 0;
}

DECLARE_LUA_FUNC(Angle_Normalize)
	FRotator r1 = self.GetNormalized();
	Lua.PushValue(1);
	LUA_SETTABLE("Pitch", Float, r1.Pitch);
	LUA_SETTABLE("Yaw", Float, r1.Yaw);
	LUA_SETTABLE("Roll", Float, r1.Roll);
	return 0;
}

DECLARE_LUA_FUNC(Angle_GridSnap, Angle)
	FRotator r1 = Lua.CheckAngle(2);
	Lua.PushAngle(self.GridSnap(r1));
	return 1;
}

DECLARE_LUA_FUNC(Angle_Add, Float, Float, Float)
	float deltaP = Lua.CheckFloat(2);
	float deltaY = Lua.CheckFloat(3);
	float deltaR = Lua.CheckFloat(4);
	FRotator r1 = self.Add(deltaP, deltaY, deltaR);
	Lua.PushValue(1);
	LUA_SETTABLE("Pitch", Float, r1.Pitch);
	LUA_SETTABLE("Yaw", Float, r1.Yaw);
	LUA_SETTABLE("Roll", Float, r1.Roll);
	return 0;
}

#undef LUA_AUTOREPLICATE

static void Ang_AddCFunctions(LuaInterface *Lua) {
	LUA_TYPEDEF_FUNC(RotateArroundAxis, Angle_RotateArroundAxis);
	LUA_TYPEDEF_FUNC(Up, Angle_Up);
	LUA_TYPEDEF_FUNC(Forward, Angle_Forward);
	LUA_TYPEDEF_FUNC(Left, Angle_Left);
	LUA_TYPEDEF_FUNC(Clamp, Angle_Clamp);
	LUA_TYPEDEF_FUNC(IsZero, Angle_IsZero);
	LUA_TYPEDEF_FUNC(ContainsNaN, Angle_ContainsNaN);
	LUA_TYPEDEF_FUNC(Equals, Angle_Equals);
	LUA_TYPEDEF_FUNC(Denormalize, Angle_Denormalize);
	LUA_TYPEDEF_FUNC(Normalize, Angle_Normalize);
	LUA_TYPEDEF_FUNC(Inverse, Angle_Inverse);
	LUA_TYPEDEF_FUNC(GridSnap, Angle_GridSnap);
	LUA_TYPEDEF_FUNC(Add, Angle_Add);	
}