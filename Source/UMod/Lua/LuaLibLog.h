#pragma once
#include "UMod.h"

class LuaEngine;

class LuaLibLog {
public:
	static void RegisterLogLib(LuaEngine *Lua);
};