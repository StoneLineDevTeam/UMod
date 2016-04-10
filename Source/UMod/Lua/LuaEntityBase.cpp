#include "UMod.h"
#include "LuaEntityBase.h"
#include "LuaEngine.h"
#include "Entities/EntityBase.h"

static TMap<FString, int> LuaEntityClasses;

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
	Lua->Lua->SetTable(-2);

	Lua->Lua->SetTable(LUA_REGISTRYINDEX); //Add Entity metatable to the registry
}

void LuaEntityBase::PushEntity(AEntityBase *Base, LuaEngine* Lua)
{
	if (Base->GetLuaRef() != LUA_NOREF) {
		Lua->Lua->PushRef(Base->GetLuaRef());
		return;
	}
	NewEntity(Base, Lua);
}
AEntityBase *LuaEntityBase::CheckEntity(int id, LuaEngine* Lua)
{

}
void LuaEntityBase::NewEntity(AEntityBase *Base, LuaEngine* Lua)
{
	Lua->Lua->NewTable(); //new Instance

	//Push the Entity table onto the stack
	Lua->Lua->PushString("Entity");
	Lua->Lua->GetTable(LUA_REGISTRYINDEX);
		
	if (PushLuaEntityClass(Base->GetClass(), Lua)) {
		Lua->Lua->PushString("__index");
		Lua->Lua->PushValue(-3);
		Lua->Lua->SetTable(-2); //LuaEntity.__index = Instance 
		Lua->Lua->SetMetaTable(-3); //setmetatble(LuaEntity, CEntity
	}
	Lua->Lua->SetMetaTable(-2); //setmetatable(CEntity, Instance)
}
void LuaEntityBase::RegisterLuaEntityClass(FString name, LuaEngine *Lua)
{
	int id = Lua->Lua->Ref();
	LuaEntityClasses.Add(name, id);
}
bool LuaEntityBase::PushLuaEntityClass(FString name, LuaEngine *Lua)
{
	int *id = LuaEntityClasses.Find(name);
	if (id == NULL) {
		return false;
	} else if (*id == LUA_NOREF) {
		return false;
	}

	Lua->Lua->PushRef(*id);
	return true;
}