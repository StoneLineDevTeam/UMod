#pragma once
#include "UMod.h"
#include "Object.h"
#include "ServerHandler.generated.h"

class UUModGameEngine;

UCLASS()
class UServerHandler : public UObject, public FNetworkNotify {
	GENERATED_BODY()

public:
	void InitHandler(UUModGameEngine *eng, FNetworkNotify *n);

	//FNetworkNotify interface
	virtual EAcceptConnection::Type NotifyAcceptingConnection() override;
	virtual void NotifyControlMessage(UNetConnection* Connection, uint8 MessageType, FInBunch& Bunch) override;
	virtual void NotifyAcceptedConnection(UNetConnection* Connection) override;
	virtual bool NotifyAcceptingChannel(class UChannel* Channel) override;
	//End
private:
	UUModGameEngine *GEngine;
	FNetworkNotify *Notify;
};