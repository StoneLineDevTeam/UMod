// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "Lua/LuaEngine.h"
#include "UModAssetsManager.h"
#include "UModGameInstance.generated.h"

//ClientSide struct
struct FConnectionStats {
	FString HostIP; //Resolved IP
	FString HostAddress; //Entered IP in main menu
	FString HostName; //Retrieved by console vars
	FString GameMode; //Server's sent game mode name
	bool ConnectionProblem; //Is there anything wrong with the connection at this time
	float SecsBeforeDisconnect; //If there's a problem with the connection, how many seconds remaining before automatic disconnect
};

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

UENUM(BlueprintType)
enum ELogCategory {
	CATEGORY_GAME,
	CATEGORY_INPUT,
	CATEGORY_LUA,
	CATEGORY_MAPS
};

UENUM(BlueprintType)
enum ELogLevel {
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR
};

class AUModCharacter;

class UUModGameEngine;

/**
 * 
 */
UCLASS()
class UUModGameInstance : public UGameInstance, public FTickableGameObject
{
	GENERATED_BODY()

	virtual void Tick(float DeltaTime);
	virtual bool IsTickable() const;
	virtual TStatId GetStatId() const;
public:
	UUModGameInstance(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Log Message", Keywords = "ue_log log"), Category = UMod_Specific)
	static void LogMessage(FString msg, ELogLevel level, ELogCategory cat);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Start Game", Keywords = "game start"), Category = UMod_Specific)
	bool StartNewGame(bool single, bool local, int32 max, FString map, FString hostName);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Net Error Message", Keywords = "get net error"), Category = UMod_Specific)
	FString GetNetErrorMessage();
	
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

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Is Dedicated", Keywords = "is dedicated"), Category = UMod_Specific)
	bool IsDedicatedServer();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Poll Server", Keywords = "server poll"), Category = UMod_Specific)
	bool PollServer(FString ip, int32 port, FString &error);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Game Engine", Keywords = "gengine game engine get"), Category = UMod_Specific)
	UUModGameEngine *GetGameEngine();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Exit Game", Keywords = "quit exit"), Category = UMod_Specific)
	static void ExitGame();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Is Listen", Keywords = "is listen"), Category = UMod_Specific)
	bool IsListenServer();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Is Editor", Keywords = "is editor"), Category = UMod_Specific)
	bool IsEditor();

	//Reloads lua (WARNING : will erase EVERY loaded files in the RAM, as it basicaly close then open the lua VM)
	void ReloadLua();

	void SetupClientConnection(FString GM, FString HN, uint8 Flags);

	virtual void Init();
	virtual void Shutdown();

	virtual void InitializeStandalone();

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);

	FConnectionStats GetConnectionInfo();

	/*Begin ServerSide library*/
	FString GetHostName(); //Will be passed to client by console vars
	FString GetGameMode(); //Will be passed to client by console vars
	/*End*/

	//Start to implement lua
	LuaEngine *Lua;

	//Assets manager
	//UPROPERTY(BlueprintReadOnly)
	UUModAssetsManager *AssetsManager;
	UPROPERTY(BlueprintReadOnly)
	class UUModConsoleManager *ConsoleManager;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Show Fatal Message", Keywords = "fatal message"), Category = UMod_Specific)
	static void ShowFatalMessage(FString content);

	static void ShowMessage(FString content);

	AUModCharacter* GetLocalPlayer();

	//Function called when UGameplayStatics::LoadStreamLevel has done
	UFUNCTION()
	void OnAsyncLevelLoadingDone();

	//Network hack
	void OnNetworkConnectionCreation(ULocalPlayer* Player);

	static bool DedicatedStatic;
	//Command to run set directly by STDInputThread
	static FString RunCMD;
	static bool ShouldRunCMD;
	//End

	//Client Timeout vars
	float ConnectTimeout;
	float Timeout;
	//End
	//Server Timeout var
	float ServerTimeout;
	//End
private:
	//Global connected host vars
	FString CurConnectedIP;
	FString CurConnectedAddress;
	bool IsConnectionHealthy;
	float InTimeOut;
	//End	

	//Server vars
	FString HostName;
	FString IP; //A bit more complicated to get
	FString GameMode;
	//End
	
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
