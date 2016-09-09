#pragma once
#include "UMod.h"
#include "Lib.h"

class LuaLibEnts : public LuaLib {
public:
	virtual void RegisterLib(LuaEngine *Lua);
	virtual bool IsClientOnly();
};

LUA_REG_APICLASS(LuaLibEnts)