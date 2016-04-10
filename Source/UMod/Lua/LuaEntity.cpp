#include "UMod.h"
#include "LuaEntity.h"
#include "LuaEngine.h"
#include "Entities/EntityBase.h"

static TMap<FString, int> LuaEntityClasses;

static int SetPos(lua_State *L) {
	return 0;
}

static int GetPos(lua_State *L) {
	return 0;
}

void LuaEntity::RegisterEntityMetaTable(LuaInterface* Lua)
{
	Lua->NewMetaTable("Entity");
	Lua->PushString("SetPos");
	Lua->PushCFunction(SetPos);
	Lua->SetTable(-2);
	Lua->PushString("GetPos");
	Lua->PushCFunction(GetPos);
	Lua->SetTable(-2);

	Lua->SetTable(LUA_REGISTRYINDEX); //Add Entity metatable to the registry
}

void LuaEntity::RegisterPlayerMetaTable(LuaInterface* Lua)
{
	Lua->NewMetaTable("Player");

	Lua->SetTable(LUA_REGISTRYINDEX); //Add Player metatable to the registry
}

void LuaEntity::PushEntity(AEntityBase *Base, LuaInterface* Lua)
{
	if (Base->GetLuaRef() != LUA_NOREF) {
		Lua->PushRef(Base->GetLuaRef());
		return;
	}
	NewEntity(Base, Lua);
}

AEntityBase *LuaEntity::CheckEntity(int id, LuaInterface* Lua)
{
	return NULL;
}

void LuaEntity::NewEntity(AEntityBase *Base, LuaInterface* Lua)
{
	Lua->NewTable(); //new Instance

	//Push the Entity table onto the stack
	Lua->PushString("Entity");
	Lua->GetTable(LUA_REGISTRYINDEX);
		
	if (PushLuaEntityClass(Base->GetClass(), Lua)) {
		Lua->PushString("__index");
		Lua->PushValue(-3);
		Lua->SetTable(-3); //LuaEntity.__index = Instance 
		Lua->SetMetaTable(-2); //setmetatble(LuaEntity, CEntity)
	}
	Lua->SetMetaTable(-2); //setmetatable(CEntity, Instance)
	
	Lua->PushString("__type");
	Lua->PushString("ENTITY");
	Lua->SetTable(-3); //Set table type for custom GetType method
	Lua->PushString("__self");
	AEntityBase** LuaBase = (AEntityBase**)Lua->NewUserData(sizeof(*Base));
	Lua->NewMetaTable("CEntity");
	Lua->SetMetaTable(-2);
	Lua->SetTable(-3); //Create and set Lua pointer
	LuaBase = &Base;

	int id = Lua->Ref(); //Create Entity reference
	Base->SetLuaRef(id);
}

void LuaEntity::RegisterLuaEntityClass(FString name, LuaInterface *Lua)
{
	int id = Lua->Ref();
	LuaEntityClasses.Add(name, id);
}

bool LuaEntity::PushLuaEntityClass(FString name, LuaInterface *Lua)
{
	int *id = LuaEntityClasses.Find(name);
	if (id == NULL) {
		return false;
	} else if (*id == LUA_NOREF) {
		return false;
	}

	Lua->PushRef(*id);
	return true;
}