#include "UMod.h"
#include "LuaEntityBase.h"
#include "LuaEngine.h"
#include "Entities/EntityBase.h"

static int SetPos(lua_State *L) {
	return 0;
}

static int GetPos(lua_State *L) {
	return 0;
}

static const luaL_Reg EntityBaseFuncs[] = {
	{"SetPos", SetPos},
	{"GetPos", GetPos}
};

void LuaEntityBase::RegisterEntityBase(LuaEngine* Lua)
{
	Lua->Lua->NewMetaTable("UEntity");
	Lua->Lua->Register(NULL, EntityBaseFuncs);
	Lua->Lua->PushValue(-1);
	Lua->Lua->PushString("__index");
	Lua->Lua->SetTable(-2);

	Lua->Lua->Register("Entity", EntityBaseFuncs);
}