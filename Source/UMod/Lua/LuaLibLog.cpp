#include "UMod.h"
#include "LuaLibLog.h"
#include "LuaEngine.h"

#define LUA_AUTOREPLICATE

/*Base log.* library*/
DECLARE_LUA_FUNC(LogInfo)
	FString msg = Lua.CheckString(1);
	UE_LOG(UMod_Lua, Log, TEXT("%s"), *msg);
	return 0;
}
DECLARE_LUA_FUNC(LogWarn)
	FString msg = Lua.CheckString(1);
	UE_LOG(UMod_Lua, Warning, TEXT("%s"), *msg);
	return 0;
}
DECLARE_LUA_FUNC(LogErr)
	FString msg = Lua.CheckString(1);
	UE_LOG(UMod_Lua, Error, TEXT("%s"), *msg);
	return 0;
}
/*End*/

#undef LUA_AUTOREPLICATE

void LuaLibLog::RegisterLib(LuaEngine *Lua)
{
	Lua->BeginLibReg("log");
	Lua->AddLibFunction("Info", LUA_LogInfo);
	Lua->AddLibFunction("Warning", LUA_LogWarn);
	Lua->AddLibFunction("Error", LUA_LogErr);
	Lua->CreateLibrary();	
}

bool LuaLibLog::IsClientOnly()
{
	return false;
}