#pragma once
#include "UMod.h"

class UUModGameEngine;

class UClientHandler : public FNetworkNotify {

public:
	void InitHandler(UUModGameEngine *eng, FNetworkNotify *n, bool Poll);

	//FNetworkNotify interface
	virtual EAcceptConnection::Type NotifyAcceptingConnection() override;
	virtual void NotifyControlMessage(UNetConnection* Connection, uint8 MessageType, FInBunch& Bunch) override;
	virtual void NotifyAcceptedConnection(UNetConnection* Connection) override;
	virtual bool NotifyAcceptingChannel(class UChannel* Channel) override;
	//End
private:
	UUModGameEngine *GEngine;
	FNetworkNotify *Notify;
	bool SendPollPacket;
};