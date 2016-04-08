#include "UMod.h"
#include "UModGameEngine.h"
#include "Engine/PendingNetGame.h"
#include "Console/UModConsoleManager.h"

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

UUModGameInstance *UUModGameEngine::GetGame()
{
	return Cast<UUModGameInstance>(GameInstance);
}

bool SendPollPacket;
void UUModGameEngine::RunPollServer(FString ip, ULocalPlayer* const Player)
{
	Player->PlayerController->ClientTravel(ip, ETravelType::TRAVEL_Absolute);
	SendPollPacket = true;
}

//FNetworkNotify interface (ClientSide)
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
FString CurRegisteringLuaFile;
FString CurLuaFileContent;
uint32 CurRegisteringLuaFileID;
void UUModGameEngine::NotifyControlMessage(UNetConnection* Connection, uint8 MessageType, class FInBunch& Bunch)
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
		Connection->Close();
		PollEndDelegate.Broadcast(res);
		break;
	}
	case NMT_UModStartVars:
		FNetControlMessage<NMT_UModStartVars>::Send(Connection);
		Connection->FlushNet();
		break;
	case NMT_UModSendVarsInt:
	{
		FString str;
		int t;
		FNetControlMessage<NMT_UModSendVarsInt>::Receive(Bunch, str, t);
		GetGame()->ConsoleManager->SetConsoleVar<int>(str, t);

		FNetControlMessage<NMT_UModEndVars>::Send(Connection);
		Connection->FlushNet();
		break;
	}
	case NMT_UModSendVarsBool:
	{
		FString str;
		bool b;
		FNetControlMessage<NMT_UModSendVarsBool>::Receive(Bunch, str, b);
		GetGame()->ConsoleManager->SetConsoleVar<bool>(str, b);

		FNetControlMessage<NMT_UModEndVars>::Send(Connection);
		Connection->FlushNet();
		break;
	}
	case NMT_UModSendVarsString:
	{
		FString str;
		FString s;
		FNetControlMessage<NMT_UModSendVarsString>::Receive(Bunch, str, s);
		GetGame()->ConsoleManager->SetConsoleVar<FString>(str, s);

		FNetControlMessage<NMT_UModEndVars>::Send(Connection);
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
				GetGame()->Disconnect("Could not save downloaded lua file " + CurRegisteringLuaFile);
			}

			GetGame()->AssetsManager->AddCLLuaFile(path, CurRegisteringLuaFile);

			CurLuaFileContent = "";

			CurRegisteringLuaFileID++;
		}		

		FNetControlMessage<NMT_UModEndLua>::Send(Connection);
		Connection->FlushNet();
		break;
	}
	case NMT_UModEndVars:
		//Server is done uploading console vars
		FNetControlMessage<NMT_UModEnd>::Send(Connection);
		Connection->FlushNet();
		break;
	case NMT_UModEndLua:
		//Server is done uploading lua
		//UMod network intializer has done all work, send a packet to confirm connection (otherwise we can just close connection)
		CurRegisteringLuaFile = "";
		CurRegisteringLuaFileID = 0;

		FNetControlMessage<NMT_UModEnd>::Send(Connection);
		Connection->FlushNet();
		break;
	default:
		Notify->NotifyControlMessage(Connection, MessageType, Bunch);
	}

	//This runs if we have a zero param message
	if (MessageType == 21 || MessageType == 23 || MessageType == 30 || MessageType == 31) {
		Bunch.SetData(Bunch, 0); //Trying to hack bunch reset pos ! Working !
		//NOTE : This may cause memory leaks, I'm not sure how UE4 handles bunches I don't know if those are getting deleted after reading.
	}
}
