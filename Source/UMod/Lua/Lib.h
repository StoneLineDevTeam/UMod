#pragma once
#include "UMod.h"
#include "LuaEngine.h"

class LuaLib {
public:
	virtual void RegisterLib(LuaEngine *Lua) = 0;
	virtual bool IsClientOnly() = 0;
	static UUModGameInstance *Game;
};

UUModGameInstance* LuaLib::Game = NULL;