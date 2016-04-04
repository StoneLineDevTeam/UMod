#pragma once

class LuaEngine;
class AEntityBase;

class LuaEntityBase {
public:
	static void RegisterEntityMetaTable(LuaEngine* Lua);
	static void PushEntity(AEntityBase *Base); //TODO : implement that method it should basicaly push an entity on the stack
	static AEntityBase *CheckEntity(int id); //TODO : implement that method it should return a valid EntityBase or Throw a lua error saying that lua gave a NULL entity !
};