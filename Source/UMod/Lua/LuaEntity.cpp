#include "UMod.h"
#include "LuaEntity.h"
#include "LuaEngine.h"
#include "Entities/EntityBase.h"

static TMap<FString, int> LuaEntityClasses;

DECLARE_LUA_FUNC(SetPos)
	AEntityBase *ent = LuaEntity::CheckEntity(1, &Lua);
	FVector pos = Lua.CheckVector(-1);
	ent->SetActorLocation(pos);
	return 0;
}

DECLARE_LUA_FUNC(SetModel)
	AEntityBase *ent = LuaEntity::CheckEntity(1, &Lua);
	FString str = Lua.CheckString(-1);
	ent->SetModel(str);
	return 0;
}

DECLARE_LUA_FUNC(GetPos)
	AEntityBase *ent = LuaEntity::CheckEntity(1, &Lua); //Get the self entity
	FVector pos = ent->GetActorLocation();
	Lua.PushVector(pos);
	return 1;
}

DECLARE_LUA_FUNC(EntIndex)
	AEntityBase *ent = LuaEntity::CheckEntity(1, &Lua); //Get the self entity	
	Lua.PushInt(ent->GetUniqueID());
	return 1;
}

DECLARE_LUA_FUNC(GetClass)
	AEntityBase *ent = LuaEntity::CheckEntity(1, &Lua); //Get the self entity
	FString s = ent->GetClass();
	Lua.PushString(s);
	return 1;
}

void LuaEntity::RegisterEntityMetaTable(LuaInterface* Lua)
{
	LUA_TYPEDEF_BEGIN(Entity);
	LUA_TYPEDEF_FUNC(SetPos, SetPos);
	LUA_TYPEDEF_FUNC(GetPos, GetPos);
	LUA_TYPEDEF_FUNC(GetClass, GetClass);
	LUA_TYPEDEF_FUNC(SetModel, SetModel);
	LUA_TYPEDEF_FUNC(EntIndex, EntIndex);
	LUA_TYPEDEF_END();	
}

void LuaEntity::RegisterPlayerMetaTable(LuaInterface* Lua)
{
	Lua->PushString("Player");
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
	Lua->PushValue(id);
	Lua->PushString("__self");
	Lua->GetTable(-2);
	AEntityBase *Base = *((AEntityBase**)Lua->CheckUserData(-1, "CEntity"));
	Lua->Pop(2);
	return Base;
}

void LuaEntity::NewEntity(AEntityBase *Base, LuaInterface* Lua)
{
	Lua->NewTable(); //new Instance

	//Push the Entity table onto the stack
	Lua->PushString("Entity");
	Lua->GetTable(LUA_REGISTRYINDEX);

	if (PushLuaEntityClass(Base->GetClass(), Lua)) { //I'll add that later
		Lua->SetMetaTable(-2); //setmetatble(LuaEntity, CEntity)
	}
	Lua->SetMetaTable(-2); //setmetatable(CEntity, Instance)
	
	Lua->PushString("__type");
	Lua->PushString("ENTITY");
	Lua->SetTable(-3); //Set table type for custom GetType method
	Lua->PushString("__self");
	AEntityBase** LuaBase = (AEntityBase**)Lua->NewUserData(sizeof(AEntityBase*));
	Lua->NewMetaTable("CEntity");
	Lua->SetMetaTable(-2);
	Lua->SetTable(-3); //Create and set Lua pointer
	*LuaBase = Base;

	int id = Lua->Ref();
	Base->SetLuaRef(id);
}

void LuaEntity::RegisterLuaEntityClass(FString name, LuaInterface *Lua)
{
	//Assuming the table we want as entity is on top of the stack
	Lua->PushString("__index"); //LuaEntity.__index = LuaEntity (found somewhere on internet, does not mean anything for me)
	Lua->PushValue(-2);
	Lua->SetTable(-3);
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