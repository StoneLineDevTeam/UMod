// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "UModGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FLuaEngineVersion {
	GENERATED_USTRUCT_BODY()

	FLuaEngineVersion()
	{
	}

	UPROPERTY(BlueprintReadOnly)
	FString LuaVersion;

	UPROPERTY(BlueprintReadOnly)
	FString LuaEngineVersion;
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

/**
 * 
 */
UCLASS()
class UMOD_API UUModGameInstance : public UGameInstance, public FTickableGameObject
{
	GENERATED_BODY()

	virtual void Tick(float DeltaTime);
	virtual bool IsTickable() const;
	virtual TStatId GetStatId() const;
public:
	UUModGameInstance(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Log Message", Keywords = "ue_log log"), Category = UMod_Specific)
		static void LogMessage(FString msg, uint8 level);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Start Game", Keywords = "game start"), Category = UMod_Specific)
		bool StartNewGame(bool single, bool local, int32 max, FString map, FString hostName);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Net Error Message", Keywords = "get net error"), Category = UMod_Specific)
		FString GetNetErrorMessage();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Num Objects To Load", Keywords = "get num objects"), Category = UMod_Specific)
		int32 GetNumObjectToLoad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Current Objects Loaded Num", Keywords = "get cur num objects"), Category = UMod_Specific)
		int32 GetCurLoadedObjectNum();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Net Load Status", Keywords = "get net load status"), Category = UMod_Specific)
		int32 GetNetLoadStatus();
		
	void SetLoadData(int32 total, int32 cur, int32 status);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Disconnect Client", Keywords = "disconnect client"), Category = UMod_Specific)
		void Disconnect(FString error);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Join Game", Keywords = "game join"), Category = UMod_Specific)
		bool JoinGame(FString ip, int32 port);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Return To MeniMenu", Keywords = "main menu return"), Category = UMod_Specific)
		void ReturnToMainMenu();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Game Version", Keywords = "game version get"), Category = UMod_Specific)
		static FString GetGameVersion();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Engine Version", Keywords = "engine version get"), Category = UMod_Specific)
		static FString GetEngineVersion();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get LuaEngine Version", Keywords = "lua engine version get"), Category = UMod_Specific)
		static FLuaEngineVersion GetLuaEngineVersion();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Map List", Keywords = "map list get"), Category = UMod_Specific)
		TArray<FUModMap> GetAllMapNames();

	virtual void Init();
	virtual void Shutdown();

	virtual void InitializeStandalone();

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);

private:
	FString CurSessionName;
	FString CurSessionMapName;
	bool IsLAN;

	bool DelayedRunMap;

	FString ConnectIP;
	bool DelayedServerConnect;

	void DestroyCurSession(IOnlineSessionPtr Sessions);

	void OnNetworkFailure(UWorld *world, UNetDriver *driver, ENetworkFailure::Type failType, const FString &ErrorMessage);

	void OnTravelFailure(UWorld *world, ETravelFailure::Type type, const FString &ErrorMessage);

	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
};
