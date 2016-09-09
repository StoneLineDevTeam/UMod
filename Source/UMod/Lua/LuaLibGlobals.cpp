#include "UMod.h"
#include "LuaLibGlobals.h"
#include "UModGameInstance.h"

/*Global methods*/
#define LUA_AUTOREPLICATE

DECLARE_LUA_FUNC(Globals_ScrSize)
	FVector2D temp;
	LuaLib::Game->GetGameViewportClient()->GetViewportSize(temp);
	Lua.PushFloat(temp.X);
	Lua.PushFloat(temp.Y);
	return 2;
}
DECLARE_LUA_FUNC(Globals_Color, Int, Int, Int)
	int r = Lua.CheckInt(1);
	int g = Lua.CheckInt(2);
	int b = Lua.CheckInt(3);
	int a = Lua.CheckInt(4);
	Lua.PushColor(FColor(r, g, b, a));
	return 1;
}
DECLARE_LUA_FUNC(Globals_Vector, Float, Float, Float)
	float x = Lua.CheckFloat(1);
	float y = Lua.CheckFloat(2);
	float z = Lua.CheckFloat(3);
	Lua.PushVector(FVector(x, y, z));
	return 1;
}
DECLARE_LUA_FUNC(Globals_Include, String)	
	FString ToInclude = Lua.CheckString(1);
	if (LuaLib::Game->IsDedicatedServer() || LuaLib::Game->IsListenServer()) { //We are a server
		LUA_ASSERT(FPaths::FileExists(FPaths::GameDir() + "/" + ToInclude));
		LuaLib::Game->Lua->RunScript(FPaths::GameDir() + "/" + ToInclude);
	} else { //We are a client
		FString real = LuaLib::Game->AssetsManager->GetLuaFile(ToInclude);
		LUA_ASSERT(FPaths::FileExists(real));
		LuaLib::Game->Lua->RunScript(real);
	}
	return 0;
}
DECLARE_LUA_FUNC(Globals_AddCSLuaFile, String)
	FString ToAdd = Lua.CheckString(1);
	LUA_ASSERT(FPaths::FileExists(FPaths::GameDir() + ToAdd));
	LuaLib::Game->AssetsManager->AddSVLuaFile(FPaths::GameDir() + ToAdd, ToAdd);	
	UE_LOG(UMod_Lua, Log, TEXT("Added lua file for upload : '%s'."), *ToAdd);
	return 0;
}
DECLARE_LUA_FUNC(Globals_HasAuthority)
	bool b = LuaLib::Game->IsDedicatedServer() || LuaLib::Game->IsListenServer();
	Lua.PushBool(b);
	return 1;
}

#undef LUA_AUTOREPLICATE
/*End*/

void LuaLibGlobals::RegisterLib(LuaEngine *Lua)
{
	//Add Color()/Vector() functions
	Lua->Lua->PushCFunction(LUA_Globals_Color);
	Lua->Lua->SetGlobal("Color");
	Lua->Lua->PushCFunction(LUA_Globals_Vector);
	Lua->Lua->SetGlobal("Vector");
	//Add Include/AddCSLuaFile functions
	Lua->Lua->PushCFunction(LUA_Globals_Include);
	Lua->Lua->SetGlobal("Include");
	Lua->Lua->PushCFunction(LUA_Globals_AddCSLuaFile);
	Lua->Lua->SetGlobal("AddCSLuaFile");
	//Add HasAuthority function
	Lua->Lua->PushCFunction(LUA_Globals_HasAuthority);
	Lua->Lua->SetGlobal("HasAuthority");
	//ScreenSize
	if (!LuaLib::Game->IsDedicatedServer()) {
		Lua->Lua->PushCFunction(LUA_Globals_ScrSize);
		Lua->Lua->SetGlobal("ScrSize");
	}
}

bool LuaLibGlobals::IsClientOnly()
{
	return false;
}