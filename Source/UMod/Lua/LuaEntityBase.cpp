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

void LuaEntityBase::RegisterEntityMetaTable(LuaEngine* Lua)
{
	Lua->Lua->NewMetaTable("Entity");
	Lua->Lua->PushString("SetPos");
	Lua->Lua->PushCFunction(SetPos);
	Lua->Lua->SetTable(-2);
	Lua->Lua->PushString("GetPos");
	Lua->Lua->PushCFunction(GetPos);
	
	Lua->Lua->SetTable(LUA_REGISTRYINDEX); //Add Entity metatable to the registry
}