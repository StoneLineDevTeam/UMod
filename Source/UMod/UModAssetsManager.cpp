// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "UModAssetsManager.h"

#include "AssetRegistryModule.h"

FUModContentPack ErrorPack = FUModContentPack(EUModContentChannel::CHANNEL_ERROR, FString("ERROR"), FString("ERROR"));

UUModAssetsManager::UUModAssetsManager(class FObjectInitializer const &)
{
	CurrentGameModeName = FString("NULL");
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
	if (!FCoreDelegates::OnMountPak.Execute(realPath, 4)) {
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

FString UUModAssetsManager::GetUnrealPath(FString VirtualPath)
{
	return "/Game/" + VirtualPath;
}

void UUModAssetsManager::UpdateTick()
{
}

void UUModAssetsManager::LoadAddonsContent()
{
	//FFileManagerGeneric FileMgr;
	
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
}

TArray<FUModMap> UUModAssetsManager::GetMapList()
{
	TArray<FUModMap> Maps;
	auto ObjectLibrary = UObjectLibrary::CreateLibrary(UWorld::StaticClass(), false, true);
	ObjectLibrary->LoadAssetDataFromPath(TEXT("/Game/Maps"));
	TArray<FAssetData> AssetDatas;
	ObjectLibrary->GetAssetDataList(AssetDatas);
	TArray<FString> Names = TArray<FString>();

	for (int i = 0; i < AssetDatas.Num(); ++i) {
		FAssetData asset = AssetDatas[i];
		FUModMap map = FUModMap(asset.AssetName.ToString(), asset.AssetName.ToString(), asset.PackageName.ToString());
		Maps.Add(map);
	}
	return Maps;
}

TArray<FUModAsset> UUModAssetsManager::GetAssetList(EUModAssetType type)
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
	}

	auto ObjectLibrary = UObjectLibrary::CreateLibrary(UWorld::StaticClass(), false, true);
	ObjectLibrary->LoadAssetDataFromPath(TEXT("/Game/Maps"));
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