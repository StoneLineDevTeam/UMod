#include "UMod.h"
#include "LuaLibEnts.h"
#include "UModGameInstance.h"

/*Add ents lib*/
#define LUA_AUTOREPLICATE

DECLARE_LUA_FUNC(Ents_Create, String)
	FString cl = Lua.CheckString(-1);
	FUModEntityClass **OutPtr = EntityClasses.Find(cl);
	if (OutPtr == NULL) {
		AEntityBase *Base = Cast<AEntityBase>(LuaLib::Game->GetWorld()->SpawnActor(AEntityBase::StaticClass())); //We are a going to spawn a Lua entity
		Base->SetLuaClass(cl);
		LuaEntity::PushEntity(Base, &Lua);
		return 1;
	}
	FUModEntityClass *Class = *OutPtr;
	if (Class->LuaSpawn) {
		Entity *Base = Class->CastToEntity(LuaLib::Game->GetWorld()->SpawnActor(Class->UEClass)); //We are going to spawn a C++ entity
		if (Base == NULL) {
			UE_LOG(UMod_Lua, Error, TEXT("Critical Engine bug : SpawnActor failure !"));
			return 0;
		}
		LuaEntity::PushEntity(Base, &Lua);
	} else {
		Lua.ThrowError("Entity '" + Class->EntClass + "'can not be spawned !");
	}
	return 1;
}
DECLARE_LUA_FUNC(Ents_Register)
	return 0;
}

#undef LUA_AUTOREPLICATE
/*End*/

void LuaLibEnts::RegisterLib(LuaEngine *Lua)
{
	Lua->BeginLibReg("ents");
	Lua->AddLibFunction("Create", LUA_Ents_Create);
	Lua->AddLibFunction("Register", LUA_Ents_Register);
	Lua->CreateLibrary();
}

bool LuaLibEnts::IsClientOnly()
{
	return false;
}