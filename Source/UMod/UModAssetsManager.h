// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UModAssetsManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPrecacheAssets);

UENUM(BlueprintType)
enum EUModAssetType {
	MATERIAL,
	TEXTURE,
	MODEL,
	SOUND,
	FONT,
	OTHER,
	MAP
};

USTRUCT(BlueprintType)
struct FUModMap {
	GENERATED_USTRUCT_BODY()

		FUModMap()
	{
	}

	FUModMap(FString a, FString b, FString c)
	{
		Path = a;
		NiceName = b;
		Category = c;
	}

	UPROPERTY(BlueprintReadOnly)
		FString Path;

	UPROPERTY(BlueprintReadOnly)
		FString NiceName;

	UPROPERTY(BlueprintReadOnly)
		FString Category;
};

USTRUCT(BlueprintType)
struct FUModAsset {
	GENERATED_USTRUCT_BODY()

		FUModAsset() {
	}

	FUModAsset(FString path, FString name) {
		Path = path;
		NiceName = name;
	}

	UPROPERTY(BlueprintReadOnly)
		FString Path;

	UPROPERTY(BlueprintReadOnly)
		FString NiceName;
};

enum EUModContentChannel {
	CHANNEL_ADDONS, //Used internally when server requests client to mount a content pack
	CHANNEL_GAMEMODE, //Used when loading GameMode custom content
	CHANNEL_LUA, //This injector is used when client side lua manualy injects custom packs (Used to prevent lua from unloading server addon's content)
	CHANNEL_ERROR
};

enum EResolverResult {
	SYNTAX_ERROR,
	INVALID_MOUNT_POINT,
	INVALID_ASSET,
	ASSET_SYSTEM_NOT_READY,
	SUCCESS
};

//Represents a content pack
struct FUModContentPack {
	FString RealPath;
	FString VirtualPath;
	EUModContentChannel InjectorChannel;

	FUModContentPack(EUModContentChannel ch, FString real, FString virt) {
		InjectorChannel = ch;
		RealPath = real;
		VirtualPath = virt;
	}

	bool operator==(FUModContentPack other)
	{
		return VirtualPath == other.VirtualPath && InjectorChannel == other.InjectorChannel && RealPath == other.RealPath;
	}		
};

//Represents a lua file as an asset with virtual directory support
struct FUModLuaAsset {
	FString RealPath;
	FString VirtualPath;
	bool ForServer;

	FUModLuaAsset(FString real, FString virt, bool server)
	{
		RealPath = real;
		VirtualPath = virt;
		ForServer = server;
	}
};

//Add missing assets path
static FString ErrorModel = "/Game/UMod/Models/Error";
static FString InvalidMaterial = "/Game/UMod/Materials/Invalid";
static FString InvalidFont = "/Game/UMod/Fonts/FederationClassic";
static FString InvalidTexture = "/Game/UMod/Textures/Invalid";

/**
 * UMod Assets Management structure
 */
//UCLASS()
class UUModAssetsManager /*: public UObject*/
{
	//GENERATED_UCLASS_BODY()

public:
	UUModAssetsManager();

	/**
	 * Injects a content pack into the game
	 * InjectorChannel is the channel you wish to use for content mounting. NOTE : EUModContentChannel::CHANNEL_ADDONS will automaticaly have a path relative to "GameDir/Addons/InnerPath". EUModContentChannel::CHANNEL_GAMEMODE will automaticaly have a path relative to "GameDir/GameModes/InnerPath"
	 */
	bool InjectContentPack(FString InnerPath, EUModContentChannel InjectorChannel);

	/**
	 * Returns an FUModContentPack to represent this pack also stored in "TArray<FUModContentPack> InjectedContentPacks;"
	 */
	FUModContentPack& GetContentPack(FString VirtualPath);
	
	/**
	 * Destroys a content pack from the game.	 
	 */
	bool DestroyContentPack(FUModContentPack &PackToDestroy);

	/**
	* Returns the UnrealEngine path for assets referenced by lua
	*/
	EResolverResult ResolveAsset(FString VirtualPath, EUModAssetType t, FString &OutUEPath);

	FString GetErrorMessage(EResolverResult res);

	/**
	* Returns the real path of a lua file by it's virtual path
	*/
	FString GetLuaFile(FString VirtualPath);

	void AddCLLuaFile(FString Path, FString Virtual);
	void AddSVLuaFile(FString Path, FString Virtual);

	TArray<FUModLuaAsset>& GetAllRegisteredFiles()
	{
		return LuaAssetFiles;
	}

	//The current gamemode name, usualy set by the GameInstance
	FString CurrentGameModeName;
	
	/**
	 * Will be called when client exits server to destroy all assets from the server (or all LuaInjected assets).
	 * Warning note that this will block the current thread until unload is done !
	 */
	void HandleServerDisconnect();

	void HandleServerConnect();

	/**
	 * Called by UModGameInstance to update this class
	 */
	void UpdateTick();

	//UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Map List", Keywords = "map list get"), Category = "UMod_Specific|AssetsManager")
	TArray<FUModMap> GetMapList();

	//UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Asset List", Keywords = "asset list get"), Category = "UMod_Specific|AssetsManager")
	TArray<FUModAsset> GetAssetList(FString MountPoint, EUModAssetType type);

	static UUModAssetsManager *Instance;

	static FPrecacheAssets PrecacheAssets;
private:
	TArray<FUModContentPack> InjectedContentPacks;
	TArray<FUModLuaAsset> LuaAssetFiles;
	TArray<FString> MountPoints;
};
