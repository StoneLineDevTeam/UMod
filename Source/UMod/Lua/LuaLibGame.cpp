#include "UMod.h"
#include "LuaLibGame.h"
#include "UModGameInstance.h"

#define LUA_AUTOREPLICATE

/*Base game.* library*/
DECLARE_LUA_FUNC(GameGetMaps)	
	Lua.NewTable();
	TArray<FUModMap> Maps = LuaLib::Game->AssetsManager->GetMapList();
	UE_LOG(UMod_Lua, Warning, TEXT("[DEBUG]Available maps : %i"), Maps.Num());
	for (int i = 0; i < Maps.Num(); i++) {
		FUModMap Map = Maps[i];
		//Reverse the index so now lua can easely do #tbl or for k, v in pairs(tbl)
		Lua.PushInt(i + 1);
		//Create a sub table that contains all data
		Lua.NewTable();
		Lua.PushString("NiceName");
		Lua.PushString(Map.NiceName);
		Lua.SetTable(-3);
		Lua.PushString("Path");
		Lua.PushString(Map.Path);
		Lua.SetTable(-3);
		Lua.PushString("Category");
		Lua.PushString(Map.Category);
		Lua.SetTable(-3);
		//Add the new sub table to the large one
		Lua.SetTable(-3);
	}
	return 1;
}
DECLARE_LUA_FUNC(GameGetAssets)
	FString mnt = Lua.CheckString(1);
	int enumIndex = Lua.CheckInt(2);
	Lua.NewTable();
	UE_LOG(UMod_Lua, Warning, TEXT("[DEBUG]Asset type to list : %i"), enumIndex);
	EUModAssetType t = EUModAssetType(enumIndex);
	TArray<FUModAsset> Assets = LuaLib::Game->AssetsManager->GetAssetList(mnt, t);
	for (int i = 0; i < Assets.Num(); i++) {
		FUModAsset Asset = Assets[i];
		//Reverse the index so now lua can easely do #tbl or for k, v in pairs(tbl)
		Lua.PushInt(i + 1);
		//Create a sub table that contains all data
		Lua.NewTable();
		Lua.PushString("NiceName");
		Lua.PushString(Asset.NiceName);
		Lua.SetTable(-3);
		Lua.PushString("Path");
		Lua.PushString(Asset.Path);
		Lua.SetTable(-3);
		//Add the new sub table to the large one
		Lua.SetTable(-3);
	}
	return 1;
}
DECLARE_LUA_FUNC(GameGetConnectionInfo)
	FConnectionStats stats = LuaLib::Game->GetConnectionInfo();
	Lua.NewTable();
	LUA_SETTABLE("GameMode", String, stats.GameMode);
	LUA_SETTABLE("HostName", String, stats.HostName);
	LUA_SETTABLE("HostAddress", String, stats.HostAddress);
	LUA_SETTABLE("HostIP", String, stats.HostIP);
	LUA_SETTABLE("ConnectionProblem", Bool, stats.ConnectionProblem);
	LUA_SETTABLE("SecsBeforeDisconnect", Float, stats.SecsBeforeDisconnect);
	return 1;
}
DECLARE_LUA_FUNC(GameGetGameMode)
	Lua.PushString(LuaLib::Game->GetGameMode());
	return 1;
}
DECLARE_LUA_FUNC(GameGetHostName)
	Lua.PushString(LuaLib::Game->GetHostName());
	return 1;
}
DECLARE_LUA_FUNC(GameGetVersion)
	Lua.PushString(LuaLib::Game->GetGameVersion());
	return 1;
}
DECLARE_LUA_FUNC(GameIsDedicated)	
	Lua.PushBool(LuaLib::Game->IsDedicatedServer());
	return 1;
}
DECLARE_LUA_FUNC(GameIsListen)	
	Lua.PushBool(LuaLib::Game->IsListenServer());
	return 1;
}
DECLARE_LUA_FUNC(GameDisconnect)	
	FString msg = Lua.CheckString(1);
	LuaLib::Game->Disconnect(msg);
	return 0;
}
DECLARE_LUA_FUNC(GameShowFatalMessage)
	FString msg = Lua.CheckString(1);
	UUModGameInstance::ShowFatalMessage(msg);
	return 0;
}
DECLARE_LUA_FUNC(GameExit)
	UUModGameInstance::ExitGame();
	return 0;
}
/*End*/

#undef LUA_AUTOREPLICATE

void LuaLibGame::RegisterLib(LuaEngine *Lua)
{
	Lua->BeginLibReg("game");
	Lua->AddLibFunction("GetMapList", LUA_GameGetMaps);
	Lua->AddLibFunction("GetAssetList", LUA_GameGetAssets);
	Lua->AddLibFunction("IsDedicated", LUA_GameIsDedicated);	
	if (LuaLib::Game->IsDedicatedServer()) {
		Lua->AddLibFunction("GetGameMode", LUA_GameGetGameMode);
		Lua->AddLibFunction("GetHostName", LUA_GameGetHostName);
	} else {
		Lua->AddLibFunction("GetConnectionInfo", LUA_GameGetConnectionInfo);
	}	
	Lua->AddLibFunction("GetVersion", LUA_GameGetVersion);
	Lua->AddLibFunction("IsListen", LUA_GameIsListen);
	Lua->AddLibFunction("Disconnect", LUA_GameDisconnect);
	Lua->AddLibFunction("ShowFatalMessage", LUA_GameShowFatalMessage);
	Lua->CreateLibrary();

	Lua->BeginLibReg("AssetType");
	Lua->AddLibConstant("MATERIAL", 0);
	Lua->AddLibConstant("TEXTURE", 1);
	Lua->AddLibConstant("MODEL", 2);
	Lua->CreateLibrary();
}

bool LuaLibGame::IsClientOnly()
{
	return false;
}