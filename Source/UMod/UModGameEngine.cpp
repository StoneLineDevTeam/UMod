#include "UMod.h"
#include "UModGameEngine.h"
#include "Engine/PendingNetGame.h"

EBrowseReturnVal::Type UUModGameEngine::Browse(FWorldContext& WorldContext, FURL URL, FString& Error)
{
	if (URL.IsInternal() && GIsClient && !URL.IsLocalInternal()) {
		//Init network
		EBrowseReturnVal::Type t = Super::Browse(WorldContext, URL, Error);
		//Hack network now (yeah fuck you UE4)
		if (t == EBrowseReturnVal::Type::Success || t == EBrowseReturnVal::Type::Pending) {
			UNetDriver *NetDriver = GEngine->FindNamedNetDriver(WorldContext.PendingNetGame, NAME_PendingNetDriver);
			//HEHEHEHHE ! You did not saw that, you are so stupid UE4 !
			Notify = NetDriver->Notify;
			NetDriver->Notify = this;
			UE_LOG(UMod_Game, Error, TEXT("Disconnected UPendingNetGame from network notify system !"));
		}
		return t;
	}
	//Ok relaying on the normal browse system as we don't want to change UE4's map loading system
	return Super::Browse(WorldContext, URL, Error);
}

EAcceptConnection::Type UUModGameEngine::NotifyAcceptingConnection()
{
	return Notify->NotifyAcceptingConnection();
}
void UUModGameEngine::NotifyAcceptedConnection(class UNetConnection* Connection)
{
	Notify->NotifyAcceptedConnection(Connection);
}
bool UUModGameEngine::NotifyAcceptingChannel(class UChannel* Channel)
{
	return Notify->NotifyAcceptingChannel(Channel);
}
void UUModGameEngine::NotifyControlMessage(UNetConnection* Connection, uint8 MessageType, class FInBunch& Bunch)
{
	//WARNING : if you don't read/discard the message it's considered unhandled and the engine will crash because it don't know what is a custom net message !
	switch (MessageType)
	{
	case NMT_UModStart:
		UE_LOG(UMod_Game, Warning, TEXT("Received message"));
		uint8 Type;
		FNetControlMessage<NMT_UModStart>::Receive(Bunch, Type);
		if (Type != 2) {
			UE_LOG(UMod_Game, Error, TEXT("NMT_UModStart : Unable to continue, server is a client !"));
			Connection->Close();
			return;
		}
		Type = 1; //Testing ServerPoll feature
		FNetControlMessage<NMT_UModStart>::Send(Connection, Type);
		Connection->FlushNet();
		break;
	case NMT_UModPoll:
	{
		FString Name;
		uint32 Cur;
		uint32 Max;
		FNetControlMessage<NMT_UModPoll>::Receive(Bunch, Name, Cur, Max);
		FServerPollResult res = FServerPollResult(Name, Cur, Max);
		PollEndDelegate.Broadcast(res);
		Connection->Close();
		break;
	}
	default:
		Notify->NotifyControlMessage(Connection, MessageType, Bunch);
	}
}
