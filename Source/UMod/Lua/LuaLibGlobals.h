#pragma once
#include "Lib.h"

class LuaLibGlobals : public LuaLib {
public:
	virtual void RegisterLib(LuaEngine *Lua);
	virtual bool IsClientOnly();
};

LUA_REG_APICLASS(LuaLibGlobals)