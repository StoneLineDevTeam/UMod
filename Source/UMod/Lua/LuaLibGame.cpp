#include "UMod.h"
#include "LuaLibGame.h"
#include "LuaEngine.h"
#include "UModGameInstance.h"

static UUModGameInstance *Game1;

/*Base game.* library*/
static int GameGetMaps(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	Lua.NewTable();
	TArray<FUModMap> Maps = Game1->AssetsManager->GetMapList();
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
static int GameGetAssets(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	int i = Lua.CheckInt(1);
	Lua.NewTable();
	UE_LOG(UMod_Lua, Warning, TEXT("[DEBUG]Asset type to list : %i"), i);
	EUModAssetType t = EUModAssetType(i);
	TArray<FUModAsset> Assets = Game1->AssetsManager->GetAssetList(t);
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
static int GameIsDedicated(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	Lua.PushBool(Game1->IsDedicatedServer());
	return 1;
}
static int GameIsListen(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	Lua.PushBool(Game1->IsDedicatedServer());
	return 1;
}
static int GameDisconnect(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FString msg = Lua.CheckString(1);
	Game1->Disconnect(msg);
	return 0;
}
static int GameShowFatalMessage(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	FString msg = Lua.CheckString(1);
	UUModGameInstance::ShowFatalMessage(msg);
	return 0;
}
static int GameExit(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	UUModGameInstance::ExitGame();
	return 0;
}
/*End*/

void LuaLibGame::RegisterGameLib(LuaEngine *Lua, UUModGameInstance *Inst)
{
	Game1 = Inst;

	Lua->BeginLibReg("game");
	Lua->AddLibFunction("GetMapList", GameGetMaps);
	Lua->AddLibFunction("GetAssetList", GameGetAssets);
	Lua->AddLibFunction("IsDedicated", GameIsDedicated);
	Lua->AddLibFunction("Disconnect", GameDisconnect);
	Lua->AddLibFunction("ShowFatalMessage", GameShowFatalMessage);
	Lua->CreateLibrary();

	Lua->BeginLibReg("AssetType");
	Lua->AddLibConstant("MATERIAL", 0);
	Lua->AddLibConstant("TEXTURE", 1);
	Lua->AddLibConstant("MODEL", 2);
	Lua->CreateLibrary();
}