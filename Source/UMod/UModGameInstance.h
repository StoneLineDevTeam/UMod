// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "UModGameInstance.generated.h"


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
	void Disconnect(FString error);

	bool JoinGame(FString ip, int port);

	virtual void Init();
	virtual void Shutdown();

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

	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
};
