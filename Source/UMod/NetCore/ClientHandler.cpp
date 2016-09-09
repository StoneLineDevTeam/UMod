#include "UMod.h"
#include "NetCore/ClientHandler.h"
#include "UModGameEngine.h"

//FNetworkNotify interface (ClientSide)
EAcceptConnection::Type UClientHandler::NotifyAcceptingConnection()
{
	return Notify->NotifyAcceptingConnection();
}
void UClientHandler::NotifyAcceptedConnection(class UNetConnection* Connection)
{
	Notify->NotifyAcceptedConnection(Connection);
}
bool UClientHandler::NotifyAcceptingChannel(class UChannel* Channel)
{
	return Notify->NotifyAcceptingChannel(Channel);
}
FString CurRegisteringLuaFile;
FString CurLuaFileContent;
uint32 CurRegisteringLuaFileID;
void UClientHandler::NotifyControlMessage(UNetConnection* Connection, uint8 MessageType, class FInBunch& Bunch)
{
	//WARNING : if you don't read/discard the message it's considered unhandled and the engine will crash because it don't know what is a custom net message !
	UE_LOG(UMod_Game, Log, TEXT("Received message with ID : %i"), (int)MessageType);
	switch (MessageType)
	{
	case NMT_UModStart:
		uint8 Type;
		FNetControlMessage<NMT_UModStart>::Receive(Bunch, Type);
		if (Type != 2) {
			UE_LOG(UMod_Game, Error, TEXT("NMT_UModStart : Unable to continue, server is a client !"));
			Connection->Close();
		}
		if (SendPollPacket) {
			Type = 1;
			SendPollPacket = false;
			UUModGameEngine::IsPollingServer = true;
		} else {
			Type = 0;

			//Create the voice channel (test purposes)
			Connection->CreateChannel(CHANNEL_VOICE, true, 5);
		}
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
		GEngine->NetworkCleanUp();
		GEngine->PollEndDelegate.Broadcast(res);
		UUModGameEngine::IsPollingServer = false;
		break;
	}
	case NMT_UModConnectVars:
	{
		//TODO : Read connect vars
		FString GameMode;
		FString HostName;
		uint8 Flags;
		FNetControlMessage<NMT_UModConnectVars>::Receive(Bunch, GameMode, HostName, Flags);
		GEngine->GetGame()->SetupClientConnection(GameMode, HostName, Flags);

		FNetControlMessage<NMT_UModEnd>::Send(Connection);
		Connection->FlushNet();
		break;
	}
	case NMT_UModStartLua:
		FNetControlMessage<NMT_UModStartLua>::Receive(Bunch, CurRegisteringLuaFile);

		FNetControlMessage<NMT_UModEndLua>::Send(Connection);
		Connection->FlushNet();
		break;
	case NMT_UModSendLua:
	{
		FString Content;
		uint8 Mode;
		FNetControlMessage<NMT_UModSendLua>::Receive(Bunch, Content, Mode);
		if (Mode == 1) {
			CurLuaFileContent += Content;
		} else if (Mode == 2) {
			CurLuaFileContent += Content;

			FString path = FPaths::GameDir() + "/Saved/LuaCache/" + FString::FromInt(CurRegisteringLuaFileID) + ".lac"; //.lac for lua asset cache
			bool b = FFileHelper::SaveStringToFile(CurLuaFileContent, *path);
			if (!b) {
				GEngine->GetGame()->Disconnect("Could not save downloaded lua file " + CurRegisteringLuaFile);
			}

			GEngine->GetGame()->AssetsManager->AddCLLuaFile(path, CurRegisteringLuaFile);

			CurLuaFileContent = "";

			CurRegisteringLuaFileID++;
		}

		FNetControlMessage<NMT_UModEndLua>::Send(Connection);
		Connection->FlushNet();
		break;
	}
	case NMT_UModEndLua:
	{
		//Server is done uploading lua
		//UMod network intializer has done all work, send a packet to confirm connection (otherwise we can just close connection)
		CurRegisteringLuaFile = "";
		CurRegisteringLuaFileID = 0;

		FNetControlMessage<NMT_UModEnd>::Send(Connection);
		Connection->FlushNet();			
		break;
	}
	case NMT_UModChangeMap:
	{
		FConnectionStats stats = GEngine->GetGame()->GetConnectionInfo();
		TArray<FString> strs;
		stats.HostIP.ParseIntoArray(strs, TEXT(":"), false);
		FString recoIP = strs[0];
		int32 recoPort = FCString::Atoi(*strs[1]);
		GEngine->NetworkCleanUp();
		GEngine->GetGame()->JoinGame(recoIP, recoPort);
		GEngine->SetLoadData(0, 0, "Server is changing map...");
		break;
	}
	default:
		Notify->NotifyControlMessage(Connection, MessageType, Bunch);
	}

	//This runs if we have a zero param message
	if (MessageType == NMT_UModEndLua || MessageType == NMT_UModEnd || MessageType == NMT_UModChangeMap) {
		Bunch.SetData(Bunch, 0); //Trying to hack bunch reset pos ! Working !		
		//NOTE : This may cause memory leaks, I'm not sure how UE4 handles bunches I don't know if those are getting deleted after reading.
	}
}

void UClientHandler::InitHandler(UUModGameEngine *eng, FNetworkNotify *n, bool Poll)
{
	GEngine = eng;
	Notify = n;
	SendPollPacket = Poll;
}