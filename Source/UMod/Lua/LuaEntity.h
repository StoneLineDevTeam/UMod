#pragma once

class LuaEngine;
class AEntityBase;
class AUModCharacter;

class LuaEntity {
public:
	static void RegisterEntityMetaTable(LuaInterface* Lua);
	static void RegisterPlayerMetaTable(LuaInterface* Lua);
	
	static void PushEntity(AEntityBase *Base, LuaInterface* Lua); //TODO : implement that method it should basicaly push an entity on the stack
	static AEntityBase *CheckEntity(int id, LuaInterface* Lua); //TODO : implement that method it should return a valid EntityBase or Throw a lua error saying that lua gave a NULL entity !
	
	static void PushPlayer(AUModCharacter *Player, LuaInterface *Lua);
	static AUModCharacter *CheckPlayer(int id, LuaInterface *Lua);	

	static void RegisterLuaEntityClass(FString name, LuaInterface *Lua); //Registers the entity class that is on top of the stack onto a TMap using an FString as key

private:
	static void NewEntity(AEntityBase *Base, LuaInterface* Lua);
	static void NewPlayer(AUModCharacter *Player, LuaInterface *Lua);
	
	static bool PushLuaEntityClass(FString name, LuaInterface *Lua); //Pushes an entity's class onto the stack
};