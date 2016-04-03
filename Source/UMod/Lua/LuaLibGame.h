#pragma once
#include "UMod.h"

class LuaEngine;
class UUModGameInstance;

class LuaLibGame {
public:
	static void RegisterGameLib(LuaEngine *Lua, UUModGameInstance *Inst);
};
