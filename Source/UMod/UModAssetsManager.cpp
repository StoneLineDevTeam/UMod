// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "UModAssetsManager.h"

#include "AssetRegistryModule.h"

FUModContentPack ErrorPack = FUModContentPack(EUModContentChannel::CHANNEL_ERROR, FString("ERROR"), FString("ERROR"));

UUModAssetsManager* UUModAssetsManager::Instance = NULL;
FPrecacheAssets UUModAssetsManager::PrecacheAssets;

UUModAssetsManager::UUModAssetsManager()
{
	CurrentGameModeName = FString("NULL");
	MountPoints.Add("UMod");
	MountPoints.Add("Internal");
}

bool UUModAssetsManager::InjectContentPack(FString InnerPath, EUModContentChannel InjectorChannel)
{
	FString ChannelLoc = "";
	FString virtualPath = FString("");	
	switch (InjectorChannel) {
	case EUModContentChannel::CHANNEL_ADDONS:
		ChannelLoc = "Addons/";
		virtualPath = "Addon";
		break;
	case EUModContentChannel::CHANNEL_GAMEMODE:
		if (CurrentGameModeName.Equals("NULL")) {
			return false;
		}
		ChannelLoc = "GameModes/" + CurrentGameModeName + "/";
		virtualPath = "GameMode." + CurrentGameModeName;
		break;
	case EUModContentChannel::CHANNEL_LUA:
		ChannelLoc = "Lua/";
		virtualPath = "Lua:";
		break;
	}
	FString realPath = FPaths::GameDir() + ChannelLoc + InnerPath + ".pak";
	
	virtualPath += ":" + InnerPath;

	FUModContentPack pack = FUModContentPack(InjectorChannel, realPath, virtualPath);
	
	if (!FCoreDelegates::OnMountPak.IsBound()) {
		UE_LOG(UMod_Game, Error, TEXT("%s mount failure, delegate does not exist !"), *virtualPath);
		return false;
	}
	if (!FCoreDelegates::OnMountPak.Execute(realPath, 4, NULL)) {
		UE_LOG(UMod_Game, Error, TEXT("%s mount failure !"), *virtualPath);
		return false;
	}

	InjectedContentPacks.Add(pack);	
	return true;
}

FUModContentPack& UUModAssetsManager::GetContentPack(FString VirtualPath)
{
	for (int i = 0; i < InjectedContentPacks.Num(); i++) {
		FUModContentPack &pack = InjectedContentPacks[i];
		return pack;
	}
	return ErrorPack;
}

EResolverResult UUModAssetsManager::ResolveAsset(FString VirtualPath, EUModAssetType t, FString &OutUEPath)
{
	//TODO : Add missing asset system
	TArray<FString> strs;
	VirtualPath.ParseIntoArray(strs, TEXT(":"), false);

	if (strs.Num() > 1) {
		FString mnt = strs[0];
		if (!MountPoints.Contains(mnt)) {
			return EResolverResult::INVALID_MOUNT_POINT;
		}
		FString path = strs[1];
		FString dir;
		switch (t) {
		case EUModAssetType::MATERIAL:
			dir = "Materials/";
			break;
		case EUModAssetType::TEXTURE:
			dir = "Textures/";
			break;
		case EUModAssetType::MODEL:
			dir = "Models/";
			break;
		case EUModAssetType::MAP:
			dir = "Maps/";
			break;
		case EUModAssetType::SOUND:
			dir = "Sounds/";
			break;
		case EUModAssetType::FONT:
			dir = "Fonts/";
			break;
		}
		bool Invalid = FPackageName::DoesPackageNameContainInvalidCharacters(FString("/Game/" + mnt + "/" + dir + path));
		if (Invalid) {
			return EResolverResult::SYNTAX_ERROR;
		}
		Invalid = !FPackageName::DoesPackageExist(FString("/Game/" + mnt + "/" + dir + path));
		if (Invalid) {
			return EResolverResult::INVALID_ASSET;
		}
		OutUEPath = "/Game/" + mnt + "/" + dir + path;
		return EResolverResult::SUCCESS;
	}

	return EResolverResult::SYNTAX_ERROR;
}

FString UUModAssetsManager::GetErrorMessage(EResolverResult res)
{
	switch (res) {
	case EResolverResult::SYNTAX_ERROR:
		return "SYNTAX_ERROR";		
	case EResolverResult::INVALID_MOUNT_POINT:
		return "INVALID_MOUNT_POINT";
	case EResolverResult::INVALID_ASSET:
		return "INVALID_ASSET";
	case EResolverResult::ASSET_SYSTEM_NOT_READY:
		return "ASSET_SYSTEM_NOT_READY";
	}
	return "UNKNOWN_ERROR";
}

void UUModAssetsManager::UpdateTick()
{
}

void UUModAssetsManager::HandleServerConnect()
{
	//Clear lua cache
	FString path = FPaths::GameDir() + "/Saved/LuaCache/";
	IFileManager::Get().DeleteDirectory(*path, true, true);
	//Recreate it !
	IFileManager::Get().MakeDirectory(*path, true);
}

bool UUModAssetsManager::DestroyContentPack(FUModContentPack &PackToDestroy)
{
	if (!FCoreDelegates::OnUnmountPak.IsBound()) {
		UE_LOG(UMod_Game, Error, TEXT("%s unmount failure, delegate does not exist !"), *PackToDestroy.VirtualPath);
		return false;
	}
	if (!FCoreDelegates::OnUnmountPak.Execute(PackToDestroy.RealPath)) {
		UE_LOG(UMod_Game, Error, TEXT("%s unmount failure !"), *PackToDestroy.VirtualPath);
		return false;
	}

	InjectedContentPacks.Remove(PackToDestroy);
	return true;
}

void UUModAssetsManager::HandleServerDisconnect()
{
	for (int i = 0; i < InjectedContentPacks.Num(); i++) {
		FUModContentPack pack = InjectedContentPacks[i];
		
		if (pack.InjectorChannel == EUModContentChannel::CHANNEL_LUA) {
			if (!FCoreDelegates::OnUnmountPak.IsBound()) {
				UE_LOG(UMod_Game, Error, TEXT("%s unmount failure, delegate does not exist !"), *pack.VirtualPath);
				return;
			}
			if (!FCoreDelegates::OnUnmountPak.Execute(pack.RealPath)) {
				UE_LOG(UMod_Game, Error, TEXT("%s unmount failure !"), *pack.VirtualPath);
				return;
			}
		}
	}

	//Clear lua cache
	FString path = FPaths::GameDir() + "/Saved/LuaCache/";
	IFileManager::Get().DeleteDirectory(*path, true, true);
	//Recreate it !
	IFileManager::Get().MakeDirectory(*path, true);

	for (int j = 0; j < LuaAssetFiles.Num(); j++) {
		FUModLuaAsset a = LuaAssetFiles[j];
		if (!a.ForServer) {
			LuaAssetFiles.RemoveAt(j);
		}
	}
}

TArray<FUModMap> UUModAssetsManager::GetMapList()
{
	TArray<FUModMap> Maps;
	for (int j = 0; j < MountPoints.Num(); j++) {
		FString mnt = MountPoints[j];
		TArray<FUModAsset> assets = GetAssetList(mnt, EUModAssetType::MAP);
		for (int i = 0; i < assets.Num(); i++) {
			FUModAsset asset = assets[j];
			Maps.Add(FUModMap(asset.NiceName, asset.NiceName, asset.Path));
		}		
	}
	return Maps;
}

TArray<FUModAsset> UUModAssetsManager::GetAssetList(FString MountPoint, EUModAssetType type)
{
	TArray<FUModAsset> Assets;
	FString searchPath;
	switch (type) {
	case EUModAssetType::MATERIAL:
		searchPath = "Materials/";
		break;
	case EUModAssetType::TEXTURE:
		searchPath = "Textures/";
		break;
	case EUModAssetType::MODEL:
		searchPath = "Models/";
		break;
	case EUModAssetType::MAP:
		searchPath = "Maps/";
		break;
	}

	auto ObjectLibrary = UObjectLibrary::CreateLibrary(UWorld::StaticClass(), false, true);
	ObjectLibrary->LoadAssetDataFromPath("/Game/" + MountPoint + "/" + searchPath);
	TArray<FAssetData> AssetDatas;
	ObjectLibrary->GetAssetDataList(AssetDatas);
	TArray<FString> Names = TArray<FString>();

	for (int i = 0; i < AssetDatas.Num(); ++i) {
		FAssetData asset = AssetDatas[i];
		FUModAsset uasset = FUModAsset(asset.ObjectPath.ToString(), asset.AssetName.ToString());
		Assets.Add(uasset);
	}
	return Assets;
}

void UUModAssetsManager::AddCLLuaFile(FString Path, FString Virtual)
{
	LuaAssetFiles.Add(FUModLuaAsset(Path, Virtual, false));
}
void UUModAssetsManager::AddSVLuaFile(FString Path, FString Virtual)
{
	LuaAssetFiles.Add(FUModLuaAsset(Path, Virtual, true));
}

FString UUModAssetsManager::GetLuaFile(FString VirtualPath)
{
	for (int i = 0; i < LuaAssetFiles.Num(); i++) {
		if (LuaAssetFiles[i].VirtualPath == VirtualPath) {
			return LuaAssetFiles[i].RealPath;
		}
	}
	return "";
}