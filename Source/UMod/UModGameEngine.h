#pragma once
#include "UMod.h"
#include "Engine/GameEngine.h"
#include "UModGameInstance.h"
#include "DataChannel.h"
#include "UModGameEngine.generated.h"

//https://docs.unrealengine.com/latest/INT/API/Runtime/Engine/GameFramework/UGameUserSettings/index.html
//Here take a look at GEngine->GameUserSettings to change render quality
//UAnimNotify to get anim notifies in pure C++

class UUModAssetsManager;

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
struct FLoadData {
	GENERATED_USTRUCT_BODY()

	FLoadData() {
	}

	FLoadData(FString s, int32 t, int32 c) {
		Text = s;
		TotalObjects = t;
		CurObjects = c;
	}

	UPROPERTY(BlueprintReadOnly)
		FString Text;

	UPROPERTY(BlueprintReadOnly)
		int32 TotalObjects;

	UPROPERTY(BlueprintReadOnly)
		int32 CurObjects;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerPollEndDelegate, FServerPollResult, ServerPollResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoadDataChangedDelegate, FLoadData, LoadData);

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

struct FPlatformStats {
	uint32 UsedMemory;
	uint32 AvailableMemory;
	float CpuUsage;
	float CpuUsedOneCore;
};

//Pre-connect packets
DEFINE_CONTROL_CHANNEL_MESSAGE_ONEPARAM(UModStart, 21, uint8); //Start UMod data (Client = {0 = Connect, 1 = ServerPoll}, Server = 2)
//DEFINE_CONTROL_CHANNEL_MESSAGE_ZEROPARAM(UModStartVars, 22); //Start sending bools and different variables like warnings, etc
//DEFINE_CONTROL_CHANNEL_MESSAGE_TWOPARAM(UModSendVarsInt, 23, FString, int32); //Send a variable
//DEFINE_CONTROL_CHANNEL_MESSAGE_TWOPARAM(UModSendVarsBool, 33, FString, bool); //Send a variable
//DEFINE_CONTROL_CHANNEL_MESSAGE_TWOPARAM(UModSendVarsString, 34, FString, FString); //Send a variable
//DEFINE_CONTROL_CHANNEL_MESSAGE_ZEROPARAM(UModEndVars, 24); //Done sending variables
DEFINE_CONTROL_CHANNEL_MESSAGE_THREEPARAM(UModConnectVars, 22, FString, FString, uint8); //GameMode, HostName, flags (unused for the moment)
DEFINE_CONTROL_CHANNEL_MESSAGE_ONEPARAM(UModStartLua, 29, FString); //Start sending a lua file
DEFINE_CONTROL_CHANNEL_MESSAGE_TWOPARAM(UModSendLua, 30, FString, uint8); //Send a line of the file (content, mode)
DEFINE_CONTROL_CHANNEL_MESSAGE_ZEROPARAM(UModEndLua, 31); //Indicates client to close the file as the upload is done
DEFINE_CONTROL_CHANNEL_MESSAGE_ZEROPARAM(UModEnd, 32); //End UMod data
//Gameplay packet
DEFINE_CONTROL_CHANNEL_MESSAGE_ZEROPARAM(UModChangeMap, 36);
//Poll packet (MC-like)
DEFINE_CONTROL_CHANNEL_MESSAGE_THREEPARAM(UModPoll, 35, FString, uint32, uint32); //Server poll (host name, cur players, max players)

class UServerHandler;
class UClientHandler;

UCLASS()
class UUModGameEngine : public UGameEngine {
	GENERATED_BODY()
public:	
	//Changes game's resolution returns true if success, false otherwise
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Change Game Resolution", Keywords = "game resolution set change"), Category = UMod_Specific)
	bool ChangeGameResolution(FUModGameResolution res);

	//Get's the current game resolution
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Game Resolution", Keywords = "game resolution get"), Category = UMod_Specific)
	FUModGameResolution GetGameResolution();

	//Get's the current game resolution
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Available Game Resolutions", Keywords = "game available resolutions get"), Category = UMod_Specific)
	TArray<FUModGameResolution> GetAvailableGameResolutions();

	void RunPollServer(FString ip, ULocalPlayer* const Player);
		
	UPROPERTY(BlueprintAssignable)
	FServerPollEndDelegate PollEndDelegate;
	UPROPERTY(BlueprintAssignable)
	FLoadDataChangedDelegate LoadDataChangeDelegate;

	virtual EBrowseReturnVal::Type Browse(FWorldContext& WorldContext, FURL URL, FString& Error);
	virtual bool LoadMap(FWorldContext & WorldContext, FURL URL, class UPendingNetGame * Pending, FString & Error);

	//Yeah let's start again with hacking ! This time UE4 didn't want to set the window title I decided, so I'm obligated to fuck it up !
	virtual void Init(class IEngineLoop* InEngineLoop) override;	

	UUModGameInstance *GetGame();

	//This is intended to be called from the client
	void NetworkCleanUp();

	static bool IsDedicated;
	static bool IsListen;
	static bool IsPollingServer;

	UUModAssetsManager *AssetsManager;
	
	void SetLoadData(int32 totalObjs, int32 curObjs, FString loadText);

	static void GetDisplayProperties(int &Width, int &Height, bool &FullScreen);
private:
	//Function called by GameInstance itself by a trickky method...
	//EDIT : No longer called by trick instead done through Init
	void OnDisplayCreated(void* Icon);

	UServerHandler *NetHandleSV;
	UClientHandler *NetHandleCL;

	FUModGameResolution CurrentGameResolution;
};