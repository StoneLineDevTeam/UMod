#pragma once
#include "UMod.h"
#include "Engine/GameEngine.h"
#include "UModGameInstance.h"
#include "UModGameEngine.generated.h"

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerPollEndDelegate, FServerPollResult, ServerPollResult);

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

class UServerHandler;
class UClientHandler;

UCLASS()
class UUModGameEngine : public UGameEngine{
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

	virtual EBrowseReturnVal::Type Browse(FWorldContext& WorldContext, FURL URL, FString& Error);

	//Yeah let's start again with hacking ! This time UE4 didn't want to set the window title I decided, so I'm obligated to fuck it up !
	virtual void Init(class IEngineLoop* InEngineLoop) override;

	UUModGameInstance *GetGame();

	//This is intended to be called from the client
	void NetworkCleanUp();

	static bool IsDedicated;
	static bool IsListen;
	static bool IsPollingServer;
private:
	//Function called by GameInstance itself by a trickky method...
	void OnDisplayCreated();

	UServerHandler *NetHandleSV;
	UClientHandler *NetHandleCL;
};