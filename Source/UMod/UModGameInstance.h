// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "Lua/LuaEngine.h"
#include "UModAssetsManager.h"
#include "DataChannel.h"
#include "UModGameInstance.generated.h"

struct FPlatformStats {
	uint32 UsedMemory;
	uint32 AvailableMemory;
	float CpuUsage;
	float CpuUsedOneCore;
};

USTRUCT(BlueprintType)
struct FServerPollResult {
	GENERATED_USTRUCT_BODY()

	FServerPollResult() {
	}

	FServerPollResult(FString s, int32 i, int32 j) {
		Name = s;
		MaxPlayers = j;
		CurPlayers = i;
	}

	UPROPERTY(BlueprintReadOnly)
	FString Name;

	UPROPERTY(BlueprintReadOnly)
	int32 MaxPlayers;

	UPROPERTY(BlueprintReadOnly)
	int32 CurPlayers;
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

USTRUCT(BlueprintType)
struct FUModGameResolution {
	GENERATED_USTRUCT_BODY()

	FUModGameResolution() {
	}
	
	FUModGameResolution(int32 width, int32 height, bool full) {
		GameWidth = width;
		GameHeight = height;
		FullScreen = full;
	}

	UPROPERTY(BlueprintReadWrite)
	int32 GameWidth;

	UPROPERTY(BlueprintReadWrite)
	int32 GameHeight;

	UPROPERTY(BlueprintReadWrite)
	bool FullScreen;

	bool operator==(FUModGameResolution other) {
		return GameWidth == other.GameWidth && GameHeight == other.GameHeight;
	}
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

//Custom control channel messages
DEFINE_CONTROL_CHANNEL_MESSAGE_ONEPARAM(UModStart, 20, uint8); //Start UMod data (Client = {0 = Connect, 1 = ServerPoll}, Server = 2)
DEFINE_CONTROL_CHANNEL_MESSAGE_ZEROPARAM(UModStartVars, 21); //Start sending bools and different variables like warnings, etc
DEFINE_CONTROL_CHANNEL_MESSAGE_TWOPARAM(UModSendVarsInt, 22, FString, int32); //Send a variable
DEFINE_CONTROL_CHANNEL_MESSAGE_TWOPARAM(UModSendVarsBool, 33, FString, bool); //Send a variable
DEFINE_CONTROL_CHANNEL_MESSAGE_TWOPARAM(UModSendVarsString, 34, FString, FString); //Send a variable
DEFINE_CONTROL_CHANNEL_MESSAGE_ZEROPARAM(UModEndVars, 23); //Done sending variables
DEFINE_CONTROL_CHANNEL_MESSAGE_ONEPARAM(UModStartLua, 24, FString); //Start sending a lua file
DEFINE_CONTROL_CHANNEL_MESSAGE_TWOPARAM(UModSendLua, 29, FString, uint8); //Send a line of the file (content, mode)
DEFINE_CONTROL_CHANNEL_MESSAGE_ZEROPARAM(UModEndLua, 30); //Indicates client to close the file as the upload is done
DEFINE_CONTROL_CHANNEL_MESSAGE_ZEROPARAM(UModEnd, 31); //End UMod data
//The poll control channel message
DEFINE_CONTROL_CHANNEL_MESSAGE_THREEPARAM(UModPoll, 32, FString, uint32, uint32); //Server poll (server name, cur players, max players)

class UUModGameEngine;

/**
 * 
 */
UCLASS()
class UMOD_API UUModGameInstance : public UGameInstance, public FTickableGameObject, public FNetworkNotify
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

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Num Objects To Load", Keywords = "get num objects"), Category = UMod_Specific)
	int32 GetNumObjectToLoad();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Current Objects Loaded Num", Keywords = "get cur num objects"), Category = UMod_Specific)
	int32 GetCurLoadedObjectNum();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Net Load Status", Keywords = "get net load status"), Category = UMod_Specific)
	int32 GetNetLoadStatus();
	
	void SetLoadData(int32 total, int32 cur, int32 status);

	//Function called by GameInstance itself by a trickky method...
	void OnDisplayCreated();

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

	//Changes game's resolution returns true if success, false otherwise
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Change Game Resolution", Keywords = "game resolution set change"), Category = UMod_Specific)
	bool ChangeGameResolution(FUModGameResolution res);

	//Get's the current game resolution
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Game Resolution", Keywords = "game resolution get"), Category = UMod_Specific)
	FUModGameResolution GetGameResolution();
	
	//Get's the current game resolution
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Available Game Resolutions", Keywords = "game available resolutions get"), Category = UMod_Specific)
	TArray<FUModGameResolution> GetAvailableGameResolutions();

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

	virtual void Init();
	virtual void Shutdown();

	virtual void InitializeStandalone();

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);

	//Get the host IP that this client is connected to
	FString GetHostIP();
	//Get the host address (input from user inside menu text box) that this client is connected to
	FString GetHostAddress();
	//Retrieve Server hostname
	FString GetHostName();
	//Retrieve Server GameMode
	FString GetGameMode();

	//Start to implement lua
	LuaEngine *Lua;

	//Assets manager
	UPROPERTY(BlueprintReadOnly)
	UUModAssetsManager *AssetsManager;
	UPROPERTY(BlueprintReadOnly)
	class UUModConsoleManager *ConsoleManager;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Show Fatal Message", Keywords = "fatal message"), Category = UMod_Specific)
	static void ShowFatalMessage(FString content);

	AUModCharacter* GetLocalPlayer();

	//Function called when UGameplayStatics::LoadStreamLevel has done
	UFUNCTION()
	void OnAsyncLevelLoadingDone();

	//Network hack
	void OnNetworkConnectionCreation(ULocalPlayer* Player);

	//FNetworkNotify interface
	virtual EAcceptConnection::Type NotifyAcceptingConnection() override;
	virtual void NotifyControlMessage(UNetConnection* Connection, uint8 MessageType, FInBunch& Bunch) override;
	virtual void NotifyAcceptedConnection(UNetConnection* Connection) override;
	virtual bool NotifyAcceptingChannel(class UChannel* Channel) override;
	//End
private:
	//Global connected host vars
	FString CurConnectedIP;
	FString CurConnectedAddress;
	FString CurConnectedName;
	//End

	//Server vars
	FString HostName;
	FString IP; //A bit more complicated to get
	FString GameMode;
	//End

	//Network system handler variables
	bool IsDedicated;
	bool IsListen;
	bool BrokeListenServer;
	FNetworkNotify *Notify;

	bool IsDisplayCreated;

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
