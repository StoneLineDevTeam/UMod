#pragma once

class LuaEngine;
class AEntityBase;

class LuaEntityBase {
public:
	static void RegisterEntityMetaTable(LuaEngine* Lua);
	static void PushEntity(AEntityBase *Base, LuaEngine* Lua); //TODO : implement that method it should basicaly push an entity on the stack
	static AEntityBase *CheckEntity(int id, LuaEngine* Lua); //TODO : implement that method it should return a valid EntityBase or Throw a lua error saying that lua gave a NULL entity !
	static void NewEntity(AEntityBase *Base, LuaEngine* Lua);
	
	static void RegisterLuaEntityClass(FString name, LuaEngine *Lua); //Registers the entity class that is on top of the stack onto a TMap using an FString as key
	static bool PushLuaEntityClass(FString name, LuaEngine *Lua); //Pushes an entity's class onto the stack
};