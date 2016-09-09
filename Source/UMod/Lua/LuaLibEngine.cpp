#include "UMod.h"
#include "LuaLibEngine.h"
#include "UModGameInstance.h"
#include "UModGameEngine.h"

#define LUA_AUTOREPLICATE

DECLARE_LUA_FUNC(EngineGetVersion)
	Lua.PushString(LuaLib::Game->GetEngineVersion());
	return 1;
}
DECLARE_LUA_FUNC(EngineChangeGameResolution)
	uint32 Width = Lua.CheckInt(1);
	uint32 Height = Lua.CheckInt(2);
	bool FullScreen = Lua.CheckBool(3);
	FUModGameResolution res(Width, Height, FullScreen);
	LuaLib::Game->GetGameEngine()->ChangeGameResolution(res);
	return 0;
}
DECLARE_LUA_FUNC(EngineGetGameResolution)
	FUModGameResolution res = LuaLib::Game->GetGameEngine()->GetGameResolution();
	Lua.NewTable();
	LUA_SETTABLE("GameWidth", Int, res.GameWidth);
	LUA_SETTABLE("GameHeight", Int, res.GameHeight);
	LUA_SETTABLE("FullScreen", Bool, res.FullScreen);
	return 1;
}
DECLARE_LUA_FUNC(EngineGetDisplayProperties)
	int W;
	int H;
	bool Full;
	UUModGameEngine::GetDisplayProperties(W, H, Full);
	Lua.PushInt(W);
	Lua.PushInt(H);
	Lua.PushBool(Full);
	return 3;
}

#undef LUA_AUTOREPLICATE

void LuaLibEngine::RegisterLib(LuaEngine *Lua)
{
	Lua->BeginLibReg("engine");
	Lua->AddLibFunction("GetVersion", LUA_EngineGetVersion);
	if (!LuaLib::Game->IsDedicatedServer()) {
		Lua->AddLibFunction("ChangeGameResolution", LUA_EngineChangeGameResolution);
		Lua->AddLibFunction("GetGameResolution", LUA_EngineGetGameResolution);
		Lua->AddLibFunction("GetDisplayProperties", LUA_EngineGetDisplayProperties);
	}
	Lua->CreateLibrary();
}

bool LuaLibEngine::IsClientOnly()
{
	return false;
}