// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "Lua/LuaEngine.h"
#include "UModAssetsManager.h"
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

	UCanvas *Current2DDrawContext;

	//Assets manager
	UPROPERTY(BlueprintReadOnly)
	UUModAssetsManager *AssetsManager;
	UPROPERTY(BlueprintReadOnly)
	class UUModConsoleManager *ConsoleManager;

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Show Fatal Message", Keywords = "fatal message"), Category = UMod_Specific)
	static void ShowFatalMessage(FString content);

	AUModCharacter* GetLocalPlayer();
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

	bool IsDedicated;

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
