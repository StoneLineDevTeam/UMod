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
			FNetControlMessage<NMT_UModStartVars>::Send(Connection);
			Connection->SetExpectedClientLoginMsgType(NMT_UModStartVars);

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
	case NMT_UModStartVars:
	{
		//Send the first variable or NMT_UModEndVars
		Connection->Challenge = "UModVars";
		Connection->ResponseId = 0;
		if (GEngine->GetGame()->ConsoleManager->ConsoleIntegers.Num() > Connection->ResponseId && GEngine->GetGame()->ConsoleManager->ConsoleIntegers[Connection->ResponseId].Synced) {
			//We have not currently reached integers limit
			int t = GEngine->GetGame()->ConsoleManager->ConsoleIntegers[Connection->ResponseId].Value;
			FString name = GEngine->GetGame()->ConsoleManager->ConsoleIntegers[Connection->ResponseId].VarName;
			FNetControlMessage<NMT_UModSendVarsInt>::Send(Connection, name, t);
			Connection->SetExpectedClientLoginMsgType(NMT_UModEndVars);
		} else {
			//We have no console vars to send
			Connection->Challenge = "UModLua";
			FNetControlMessage<NMT_UModEndVars>::Send(Connection);
			Connection->SetExpectedClientLoginMsgType(NMT_UModEnd);
		}
		Connection->FlushNet();
		break;
	}
	case NMT_UModEndVars:
	{
		Connection->ResponseId++;
		//Send the next variable or NMT_UModEndVars in case nothing else
		if (GEngine->GetGame()->ConsoleManager->ConsoleIntegers.Num() > Connection->ResponseId && GEngine->GetGame()->ConsoleManager->ConsoleIntegers[Connection->ResponseId].Synced) {
			//We have not currently reached integers limit
			int t = GEngine->GetGame()->ConsoleManager->ConsoleIntegers[Connection->ResponseId].Value;
			FString name = GEngine->GetGame()->ConsoleManager->ConsoleIntegers[Connection->ResponseId].VarName;
			FNetControlMessage<NMT_UModSendVarsInt>::Send(Connection, name, t);
			Connection->SetExpectedClientLoginMsgType(NMT_UModEndVars);
		} else if (GEngine->GetGame()->ConsoleManager->ConsoleBooleans.Num() + GEngine->GetGame()->ConsoleManager->ConsoleIntegers.Num() > Connection->ResponseId && GEngine->GetGame()->ConsoleManager->ConsoleBooleans[Connection->ResponseId].Synced) {
			//We have reached integer limit but not bool limit
			bool b = GEngine->GetGame()->ConsoleManager->ConsoleBooleans[Connection->ResponseId].Value;
			FString name = GEngine->GetGame()->ConsoleManager->ConsoleBooleans[Connection->ResponseId].VarName;
			FNetControlMessage<NMT_UModSendVarsBool>::Send(Connection, name, b);
			Connection->SetExpectedClientLoginMsgType(NMT_UModEndVars);
		} else {
			Connection->Challenge = "UModLua";
			Connection->ResponseId = 0;
			FNetControlMessage<NMT_UModEndVars>::Send(Connection);
			Connection->SetExpectedClientLoginMsgType(NMT_UModEnd);
		}
		Connection->FlushNet();
		break;
	}
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
		if (Connection->Challenge == "UModLua") {
			//TODO : Start sending first lua file
			if (GEngine->GetGame()->AssetsManager->GetAllRegisteredFiles().Num() > Connection->ResponseId) {
				Connection->Challenge = "UModLua_Start";
				FString str = GEngine->GetGame()->AssetsManager->GetAllRegisteredFiles()[Connection->ResponseId].VirtualPath;
				FNetControlMessage<NMT_UModStartLua>::Send(Connection, str);
				Connection->SetExpectedClientLoginMsgType(NMT_UModEndLua);
				Connection->FlushNet();
			}
			else {
				Connection->Challenge = "";
				Connection->ResponseId = 0;
				FNetControlMessage<NMT_UModEndLua>::Send(Connection);
				Connection->SetExpectedClientLoginMsgType(NMT_UModEnd);
				Connection->FlushNet();
			}
		}
		else {
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
	if (MessageType == 21 || MessageType == 23 || MessageType == 30 || MessageType == 31) {
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