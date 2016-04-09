#pragma once
#include "LuaInterface.h"
#include "UMod.h"

static int Add(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	return 0;
}

static void Ang_AddCFunctions(LuaInterface *Lua) {

}