#include "UMod.h"
#include "NetCore/ServerHandler.h"
#include "UModGameEngine.h"

//FNetworkNotify interface (ServerSide)
void UServerHandler::NotifyControlMessage(UNetConnection* Connection, uint8 MessageType, FInBunch& Bunch)
{
	//WARNING : if you don't read/discard the message it's considered unhandled and the engine will crash because it don't know what is a custom net message !
	switch (MessageType) {
	case NMT_Hello:
	{
		uint8 LittleEndian = 0;
		uint32 ClientVersion = 0;
		uint32 ServerVersion = FNetworkVersion::GetLocalNetworkVersion();

		FNetControlMessage<NMT_Hello>::Receive(Bunch, LittleEndian, ClientVersion);

		if (!FNetworkVersion::IsNetworkCompatible(ClientVersion, ServerVersion)) {
			UE_LOG(UMod_Game, Error, TEXT("NMT_Hello : Client connecting with invalid version."));
			FNetControlMessage<NMT_Upgrade>::Send(Connection, ServerVersion);
			Connection->FlushNet(true);
			Connection->Close();
		} else {
			Connection->SetExpectedClientLoginMsgType(NMT_UModStart);
			uint8 Type = 2;
			FNetControlMessage<NMT_UModStart>::Send(Connection, Type);
		}
		break;
	}
	case NMT_UModStart:
		uint8 ConnectType;
		FNetControlMessage<NMT_UModStart>::Receive(Bunch, ConnectType);
		if (ConnectType == 0) {
			uint8 flags = 0;
			FString GM = GEngine->GetGame()->GetGameMode();
			FString HN = GEngine->GetGame()->GetHostName();
			FNetControlMessage<NMT_UModConnectVars>::Send(Connection, GM, HN, flags);
			Connection->FlushNet(true);
			Connection->Challenge = "UModLua";
			Connection->SetExpectedClientLoginMsgType(NMT_UModEnd);

			//Create the voice channel (test purposes)
			Connection->CreateChannel(CHANNEL_VOICE, true, 5);
		} else if (ConnectType == 1) {
			uint32 cur = 0;
			uint32 max = 0;
			FString str = GEngine->GetGame()->GetHostName();
			FNetControlMessage<NMT_UModPoll>::Send(Connection, str, cur, max);
			Connection->FlushNet(true);
		} else if (ConnectType == 2) {
			UE_LOG(UMod_Game, Error, TEXT("NMT_UModStart : Unable to continue, client is a server !"));
			Connection->FlushNet(true);
			Connection->Close();
		}
		break;
	case NMT_UModEndLua:
		//TODO : Send the next lua file or UModEndLua if no more files
		if (Connection->Challenge == "UModLua_End") {
			Connection->ResponseId++;
			if (GEngine->GetGame()->AssetsManager->GetAllRegisteredFiles().Num() > Connection->ResponseId) {
				Connection->Challenge = "UModLua_Start";
				FString str = GEngine->GetGame()->AssetsManager->GetAllRegisteredFiles()[Connection->ResponseId].VirtualPath;
				FNetControlMessage<NMT_UModStartLua>::Send(Connection, str);
				Connection->SetExpectedClientLoginMsgType(NMT_UModEndLua);
				Connection->FlushNet();
			} else {
				Connection->Challenge = "";
				Connection->ResponseId = 0;
				FNetControlMessage<NMT_UModEndLua>::Send(Connection);
				Connection->SetExpectedClientLoginMsgType(NMT_UModEnd);
				Connection->FlushNet();
			}
		} else {
			FString str;
			uint8 Mode;
			bool b = true;
			if (Connection->Challenge == "UModLua_Start") {
				b = FFileHelper::LoadFileToString(str, *GEngine->GetGame()->AssetsManager->GetAllRegisteredFiles()[Connection->ResponseId].RealPath, 0);
				Connection->Challenge = str;
			}
			if (!b) {
				FString fuck = "Error while loading lua file into string " + GEngine->GetGame()->AssetsManager->GetAllRegisteredFiles()[Connection->ResponseId].RealPath;
				FNetControlMessage<NMT_Failure>::Send(Connection, fuck);
				Connection->FlushNet(true);
			} else {
				if (Connection->Challenge.Len() > NAME_SIZE - 1) {
					FString Content = Connection->Challenge.Mid(0, NAME_SIZE - 1);
					Connection->Challenge = Connection->Challenge.Mid(Content.Len(), Connection->Challenge.Len() - Content.Len());
					Mode = 1; //Send middle part
					FNetControlMessage<NMT_UModSendLua>::Send(Connection, Content, Mode);
					Connection->SetExpectedClientLoginMsgType(NMT_UModEndLua);
					Connection->FlushNet();
				} else {
					Mode = 2; //Send the last part of the file
					FNetControlMessage<NMT_UModSendLua>::Send(Connection, Connection->Challenge, Mode);
					Connection->Challenge = "UModLua_End";
					Connection->SetExpectedClientLoginMsgType(NMT_UModEndLua);
					Connection->FlushNet();
				}
			}
		}
		break;
	case NMT_UModEnd:
		UE_LOG(UMod_Game, Log, TEXT("[DEBUG]UModEnd Challenge : %s"), *Connection->Challenge);
		if (Connection->Challenge == "UModLua") {
			//TODO : Start sending first lua file
			if (GEngine->GetGame()->AssetsManager->GetAllRegisteredFiles().Num() > Connection->ResponseId) {
				Connection->Challenge = "UModLua_Start";
				FString str = GEngine->GetGame()->AssetsManager->GetAllRegisteredFiles()[Connection->ResponseId].VirtualPath;
				FNetControlMessage<NMT_UModStartLua>::Send(Connection, str);
				Connection->SetExpectedClientLoginMsgType(NMT_UModEndLua);
				Connection->FlushNet();
			} else {
				Connection->Challenge = "";
				Connection->ResponseId = 0;
				FNetControlMessage<NMT_UModEndLua>::Send(Connection);
				Connection->SetExpectedClientLoginMsgType(NMT_UModEnd);
				Connection->FlushNet();
			}
		} else {
			//We are ready to resume UE4 normal connection system
			Connection->Challenge = FString::Printf(TEXT("%08X"), FPlatformTime::Cycles());
			Connection->SetExpectedClientLoginMsgType(NMT_Login);
			FNetControlMessage<NMT_Challenge>::Send(Connection, Connection->Challenge);
			Connection->FlushNet();
		}
		break;
	default:
		Notify->NotifyControlMessage(Connection, MessageType, Bunch);
	}

	//This runs if we have a zero param message
	if (MessageType == NMT_UModEndLua || MessageType == NMT_UModEnd) {
		Bunch.SetData(Bunch, 0); //Trying to hack bunch reset pos ! Working !
		//NOTE : This may cause memory leaks, I'm not sure how UE4 handles bunches I don't know if those are getting deleted after reading.
	}
}
void UServerHandler::NotifyAcceptedConnection(UNetConnection* Connection)
{
	//ServerSide
	Notify->NotifyAcceptedConnection(Connection);
}
EAcceptConnection::Type UServerHandler::NotifyAcceptingConnection()
{
	return Notify->NotifyAcceptingConnection(); //ServerSide
}
bool UServerHandler::NotifyAcceptingChannel(UChannel* Channel)
{
	return Notify->NotifyAcceptingChannel(Channel); //ServerSide
}
//End

void UServerHandler::InitHandler(UUModGameEngine *eng, FNetworkNotify *n)
{
	GEngine = eng;
	Notify = n;
}