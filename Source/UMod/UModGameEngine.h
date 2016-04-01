#pragma once
#include "UMod.h"
#include "Engine/GameEngine.h"
#include "UModGameInstance.h"
#include "UModGameEngine.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerPollEndDelegate, FServerPollResult, ServerPollResult);

UCLASS()
class UUModGameEngine : public UGameEngine, public FNetworkNotify {
	GENERATED_BODY()
public:
	void RunPollServer(FString ip, ULocalPlayer* const Player);

	UPROPERTY(BlueprintAssignable)
	FServerPollEndDelegate PollEndDelegate;

	virtual EBrowseReturnVal::Type Browse(FWorldContext& WorldContext, FURL URL, FString& Error);

	//FUCK YOU UE4 YOU BROKE MY ASS ! I'M IMPATIENT, SO FUCK OFF AND THERE WE GO
	virtual EAcceptConnection::Type NotifyAcceptingConnection() override;
	virtual void NotifyAcceptedConnection(class UNetConnection* Connection) override;
	virtual bool NotifyAcceptingChannel(class UChannel* Channel) override;
	virtual void NotifyControlMessage(UNetConnection* Connection, uint8 MessageType, class FInBunch& Bunch) override;
	//END

	UUModGameInstance *GetGame(UNetConnection *Connection);
private:
	FNetworkNotify *Notify;
};