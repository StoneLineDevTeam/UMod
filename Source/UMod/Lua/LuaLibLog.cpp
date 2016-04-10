#include "UMod.h"
#include "LuaLibLog.h"
#include "LuaEngine.h"

/*Base log.* library*/
static int LogInfo(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FString msg = Lua.CheckString(1);
	UE_LOG(UMod_Lua, Log, TEXT("%s"), *msg);
	return 0;
}
static int LogWarn(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FString msg = Lua.CheckString(1);
	UE_LOG(UMod_Lua, Warning, TEXT("%s"), *msg);
	return 0;
}
static int LogErr(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FString msg = Lua.CheckString(1);
	UE_LOG(UMod_Lua, Error, TEXT("%s"), *msg);
	return 0;
}
/*End*/

void LuaLibLog::RegisterLogLib(LuaEngine *Lua)
{
	Lua->BeginLibReg("log");
	Lua->AddLibFunction("Info", LogInfo);
	Lua->AddLibFunction("Warning", LogWarn);
	Lua->AddLibFunction("Error", LogErr);
	Lua->CreateLibrary();
}