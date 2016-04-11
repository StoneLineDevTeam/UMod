// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "UModGameInstance.h"

#include "Player/UModPlayerState.h"

#include "Game/UModGameMode.h"
#include "Game/MenuGameMode.h"

#include "Console/ConsoleDestroyer.h"
#include "Console/UModConsoleManager.h"

#include "Player/UModCharacter.h"

#include "UModGameEngine.h"

//Custom control channel messages
IMPLEMENT_CONTROL_CHANNEL_MESSAGE(UModStart);
IMPLEMENT_CONTROL_CHANNEL_MESSAGE(UModStartVars);
IMPLEMENT_CONTROL_CHANNEL_MESSAGE(UModSendVarsBool);
IMPLEMENT_CONTROL_CHANNEL_MESSAGE(UModSendVarsInt);
IMPLEMENT_CONTROL_CHANNEL_MESSAGE(UModSendVarsString);
IMPLEMENT_CONTROL_CHANNEL_MESSAGE(UModEndVars);
IMPLEMENT_CONTROL_CHANNEL_MESSAGE(UModStartLua);
IMPLEMENT_CONTROL_CHANNEL_MESSAGE(UModSendLua);
IMPLEMENT_CONTROL_CHANNEL_MESSAGE(UModEndLua);
IMPLEMENT_CONTROL_CHANNEL_MESSAGE(UModEnd);
//The poll control channel message
IMPLEMENT_CONTROL_CHANNEL_MESSAGE(UModPoll);

const FString GVersion = FString("0.4 - Alpha");
FString LuaVersion;
const FString LuaEngineVersion = FString("NULL");
static bool DedicatedStatic;

const uint32 MinResX = 1200;
const uint32 MinResY = 650;

//WARNING : Client side only bool
bool IsPollingServer;

UUModGameInstance::UUModGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UUModGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UUModGameInstance::OnStartOnlineGameComplete);

	AssetsManager = NewObject<UUModAssetsManager>();
}

//FNetworkNotify interface (ServerSide)
void UUModGameInstance::NotifyControlMessage(UNetConnection* Connection, uint8 MessageType, FInBunch& Bunch)
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
			FString str = GetHostName();
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
		if (ConsoleManager->ConsoleIntegers.Num() > Connection->ResponseId && ConsoleManager->ConsoleIntegers[Connection->ResponseId].Synced) {
			//We have not currently reached integers limit
			int t = ConsoleManager->ConsoleIntegers[Connection->ResponseId].Value;
			FString name = ConsoleManager->ConsoleIntegers[Connection->ResponseId].VarName;
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
		if (ConsoleManager->ConsoleIntegers.Num() > Connection->ResponseId && ConsoleManager->ConsoleIntegers[Connection->ResponseId].Synced) {
			//We have not currently reached integers limit
			int t = ConsoleManager->ConsoleIntegers[Connection->ResponseId].Value;
			FString name = ConsoleManager->ConsoleIntegers[Connection->ResponseId].VarName;
			FNetControlMessage<NMT_UModSendVarsInt>::Send(Connection, name, t);
			Connection->SetExpectedClientLoginMsgType(NMT_UModEndVars);
		} else if (ConsoleManager->ConsoleBooleans.Num() + ConsoleManager->ConsoleIntegers.Num() > Connection->ResponseId && ConsoleManager->ConsoleBooleans[Connection->ResponseId].Synced) {
			//We have reached integer limit but not bool limit
			bool b = ConsoleManager->ConsoleBooleans[Connection->ResponseId].Value;
			FString name = ConsoleManager->ConsoleBooleans[Connection->ResponseId].VarName;
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
			if (AssetsManager->GetAllRegisteredFiles().Num() > Connection->ResponseId) {
				Connection->Challenge = "UModLua_Start";
				FString str = AssetsManager->GetAllRegisteredFiles()[Connection->ResponseId].VirtualPath;
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
				b = FFileHelper::LoadFileToString(str, *AssetsManager->GetAllRegisteredFiles()[Connection->ResponseId].RealPath, 0);
				Connection->Challenge = str;
			}
			if (!b) {
				FString fuck = "Error while loading lua file into string " + AssetsManager->GetAllRegisteredFiles()[Connection->ResponseId].RealPath;
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
			if (AssetsManager->GetAllRegisteredFiles().Num() > Connection->ResponseId) {
				Connection->Challenge = "UModLua_Start";
				FString str = AssetsManager->GetAllRegisteredFiles()[Connection->ResponseId].VirtualPath;
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
	if (MessageType == 21 || MessageType == 23 || MessageType == 30 || MessageType == 31) {
		Bunch.SetData(Bunch, 0); //Trying to hack bunch reset pos ! Working !
		//NOTE : This may cause memory leaks, I'm not sure how UE4 handles bunches I don't know if those are getting deleted after reading.
	}
}
void UUModGameInstance::NotifyAcceptedConnection(UNetConnection* Connection)
{
	//ServerSide
	Notify->NotifyAcceptedConnection(Connection);
}
EAcceptConnection::Type UUModGameInstance::NotifyAcceptingConnection()
{
	return Notify->NotifyAcceptingConnection(); //ServerSide
}
bool UUModGameInstance::NotifyAcceptingChannel(UChannel* Channel)
{
	return Notify->NotifyAcceptingChannel(Channel); //ServerSide
}
//End

//Network hack
void UUModGameInstance::OnNetworkConnectionCreation(ULocalPlayer *Player)
{
	//Yeah no longer need for hacky thing as now UUModGameEngine actualy does this stuff inside Browse
	Player->PlayerController->ClientTravel(ConnectIP, ETravelType::TRAVEL_Absolute);
}
//End

void UUModGameInstance::LogMessage(FString msg, ELogLevel level, ELogCategory cat)
{
	switch (level)
	{
	case ELogLevel::LOG_INFO:
		switch (cat)
		{
		case ELogCategory::CATEGORY_GAME:
			UE_LOG(UMod_Game, Log, TEXT("%s"), *msg);
			break;
		case ELogCategory::CATEGORY_INPUT:
			UE_LOG(UMod_Input, Log, TEXT("%s"), *msg);
			break;
		case ELogCategory::CATEGORY_LUA:
			UE_LOG(UMod_Lua, Log, TEXT("%s"), *msg);
			break;
		case ELogCategory::CATEGORY_MAPS:
			UE_LOG(UMod_Maps, Log, TEXT("%s"), *msg);
			break;
		}		
		break;
	case ELogLevel::LOG_WARNING:
		switch (cat)
		{
		case ELogCategory::CATEGORY_GAME:
			UE_LOG(UMod_Game, Warning, TEXT("%s"), *msg);
			break;
		case ELogCategory::CATEGORY_INPUT:
			UE_LOG(UMod_Input, Warning, TEXT("%s"), *msg);
			break;
		case ELogCategory::CATEGORY_LUA:
			UE_LOG(UMod_Lua, Warning, TEXT("%s"), *msg);
			break;
		case ELogCategory::CATEGORY_MAPS:
			UE_LOG(UMod_Maps, Warning, TEXT("%s"), *msg);
			break;
		}
		break;
	case ELogLevel::LOG_ERROR:
		switch (cat)
		{
		case ELogCategory::CATEGORY_GAME:
			UE_LOG(UMod_Game, Error, TEXT("%s"), *msg);
			break;
		case ELogCategory::CATEGORY_INPUT:
			UE_LOG(UMod_Input, Error, TEXT("%s"), *msg);
			break;
		case ELogCategory::CATEGORY_LUA:
			UE_LOG(UMod_Lua, Error, TEXT("%s"), *msg);
			break;
		case ELogCategory::CATEGORY_MAPS:
			UE_LOG(UMod_Maps, Error, TEXT("%s"), *msg);
			break;
		}
		break;
	}
}

FString UUModGameInstance::GetGameVersion()
{
	return GVersion;
}

FString UUModGameInstance::GetEngineVersion()
{
	int32 vers = GEngine->GetLinkerUE4Version();

	return FString::FromInt(vers);
}

FLuaEngineVersion UUModGameInstance::GetLuaEngineVersion()
{
	FLuaEngineVersion l = FLuaEngineVersion();	
	l.LuaVersion = LuaVersion;
	l.LuaEngineVersion = LuaEngineVersion;
	return l;
}

UUModGameEngine *UUModGameInstance::GetGameEngine()
{
	return Cast<UUModGameEngine>(GEngine);
}

void UUModGameInstance::OnNetworkFailure(UWorld *world, UNetDriver *driver, ENetworkFailure::Type failType, const FString &ErrorMessage)
{
	if (!IsPollingServer && !IsDedicated) {
		UE_LOG(UMod_Game, Error, TEXT("Network error occured !"));

		FString err = ErrorMessage;
		Disconnect(err);
		if (IsPollingServer) {
			IsPollingServer = false;
		}
	}
}

void UUModGameInstance::OnTravelFailure(UWorld *world, ETravelFailure::Type type, const FString &ErrorMessage)
{
	UE_LOG(UMod_Game, Error, TEXT("Travel error occured !"));

	FString err = ErrorMessage;
	Disconnect(err);
}

void UUModGameInstance::InitializeStandalone()
{	
}

void UUModGameInstance::Init()
{
	//Console log retriever hack
	ConsoleManager = NewObject<UUModConsoleManager>();
	GLog->AddOutputDevice(ConsoleManager);
	GLog->SerializeBacklog(ConsoleManager);
	GLog->EnableBacklog(true);
	//End

	int32 vers = GEngine->GetLinkerUE4Version();
	UE_LOG(UMod_Game, Log, TEXT("UMod - V.%s | Engine V.%s"), *GetGameVersion(), *FString::FromInt(vers));

	GEngine->ConsoleClass = UConsoleDestroyer::StaticClass();

	GEngine->NetworkFailureEvent.AddUObject(this, &UUModGameInstance::OnNetworkFailure);
	GEngine->TravelFailureEvent.AddUObject(this, &UUModGameInstance::OnTravelFailure);

	/*Hack NET vars*/
	//Found a way to set port !
	int32 ServerPort = 0;
	GConfig->GetInt(TEXT("Common"), TEXT("Port"), ServerPort, FPaths::GameConfigDir() + FString("UMod.Server.cfg"));
	if (ServerPort == 0) {
		ServerPort = 25565;
		GConfig->SetInt(TEXT("Common"), TEXT("Port"), ServerPort, FPaths::GameConfigDir() + FString("UMod.Server.cfg"));
	}
	GConfig->SetInt(TEXT("URL"), TEXT("Port"), ServerPort, GEngineIni);
	//Found a hacky way to set the timeout and the connection timeout
	float ConnectTimeout = 0;
	float Timeout = 0;
	GConfig->GetFloat(TEXT("Common"), TEXT("ConnectTimeout"), ConnectTimeout, FPaths::GameConfigDir() + FString("UMod.Server.cfg"));
	GConfig->GetFloat(TEXT("Common"), TEXT("Timeout"), Timeout, FPaths::GameConfigDir() + FString("UMod.Server.cfg"));
	if (ConnectTimeout == 0) {
		ConnectTimeout = 5;
		GConfig->SetFloat(TEXT("Common"), TEXT("ConnectTimeout"), ConnectTimeout, FPaths::GameConfigDir() + FString("UMod.Server.cfg"));
	}
	if (Timeout == 0) {
		Timeout = 45;
		GConfig->SetFloat(TEXT("Common"), TEXT("Timeout"), Timeout, FPaths::GameConfigDir() + FString("UMod.Server.cfg"));
	}
	GConfig->SetFloat(TEXT("/Script/OnlineSubsystemUtils.OnlineBeacon"), TEXT("BeaconConnectionInitialTimeout"), ConnectTimeout, GEngineIni);
	GConfig->SetFloat(TEXT("/Script/OnlineSubsystemUtils.OnlineBeacon"), TEXT("BeaconConnectionTimeout"), Timeout, GEngineIni);
	GConfig->SetFloat(TEXT("/Script/Lobby.LobbyBeaconClient"), TEXT("BeaconConnectionInitialTimeout"), ConnectTimeout, GEngineIni);
	GConfig->SetFloat(TEXT("/Script/Lobby.LobbyBeaconClient"), TEXT("BeaconConnectionTimeout"), Timeout, GEngineIni);
	//Retrieve host name
	GConfig->GetString(TEXT("Common"), TEXT("HostName"), HostName, FPaths::GameConfigDir() + FString("UMod.Server.cfg"));
	if (HostName.IsEmpty()) {
		HostName = "A UMod Server";
		GConfig->SetString(TEXT("Common"), TEXT("HostName"), *HostName, FPaths::GameConfigDir() + FString("UMod.Server.cfg"));
	}
	//Retrieve lua GameMode
	GConfig->GetString(TEXT("Common"), TEXT("GameMode"), GameMode, FPaths::GameConfigDir() + FString("UMod.Server.cfg"));
	if (GameMode.IsEmpty()) {
		GameMode = FString("Sandbox");
		GConfig->SetString(TEXT("Common"), TEXT("GameMode"), *GameMode, FPaths::GameConfigDir() + FString("UMod.Server.cfg"));
	}
	/*End*/

	FString str = FCommandLine::Get();
	TArray<FString> cmds;
	str.ParseIntoArray(cmds, TEXT(" "));	
	if (cmds.Contains("-server")) {
		//Retrieve map name from config
		FString MapToLoad;
		GConfig->GetString(TEXT("Dedicated"), TEXT("Map"), MapToLoad, FPaths::GameConfigDir() + FString("UMod.Server.cfg"));
		if (MapToLoad.IsEmpty()) {
			MapToLoad = FString("FirstPersonExampleMap");
			GConfig->SetString(TEXT("Dedicated"), TEXT("Map"), *MapToLoad, FPaths::GameConfigDir() + FString("UMod.Server.cfg"));
		}

		const UGameMapsSettings* GameMapsSettings = GetDefault<UGameMapsSettings>();
		GameMapsSettings->SetGameDefaultMap("/Game/Maps/" + MapToLoad);
		GameMapsSettings->SetGlobalDefaultGameMode("UModGameMode");

		GLogConsole->Show(true);
		GLogConsole->SetSuppressEventTag(false);

		FString str = "/Game/Maps/" + MapToLoad + " -server -log";
		FCommandLine::Set(*str);

		IsDedicated = true;
		DedicatedStatic = true;
	} else {
		const UGameMapsSettings* GameMapsSettings = GetDefault<UGameMapsSettings>();
		GameMapsSettings->SetGameDefaultMap("/Game/Internal/Maps/MainMenu");

		IsDedicated = false;
	}

	UE_LOG(UMod_Game, Log, TEXT("UMod - Starting Lua Engine..."));
	Lua = new LuaEngine(this);
	if (!IsEditor()) {
		LuaVersion = Lua->GetLuaVersion();
		FString lua = FString("LuaEngine V.") + LuaEngineVersion + FString(" | Lua V.") + LuaVersion;
		UE_LOG(UMod_Lua, Log, TEXT("%s"), *lua);

		Lua->RunScript(FPaths::GameDir() + FString("UMod.lua"));
		//Lua->RunScriptFunctionTwoParam<int, FString>(ETableType::GAMEMODE, 0, "Initialize", FLuaParam<int>(25), FLuaParam<FString>(lua));
	}
}

void UUModGameInstance::OnDisplayCreated()
{
	int32 width = MinResX;
	int32 height = MinResY;
	bool full;
	GConfig->GetInt(TEXT("Viewport"), TEXT("Width"), width, FPaths::GameConfigDir() + FString("UMod.Client.cfg"));
	GConfig->GetInt(TEXT("Viewport"), TEXT("Height"), height, FPaths::GameConfigDir() + FString("UMod.Client.cfg"));
	GConfig->GetBool(TEXT("Viewport"), TEXT("FullScreen"), full, FPaths::GameConfigDir() + FString("UMod.Client.cfg"));
	if (ChangeGameResolution(FUModGameResolution(width, height, full))) {
		UE_LOG(UMod_Game, Log, TEXT("Applied saved game resolution."));
	} else {
		UE_LOG(UMod_Game, Log, TEXT("Error applying saved game resolution."));
	}
}

bool UUModGameInstance::IsDedicatedServer()
{
	return IsDedicated;
}

//Game shutdown
void UUModGameInstance::Shutdown()
{
	delete Lua;
}

void UUModGameInstance::DestroyCurSession(IOnlineSessionPtr Sessions)
{
	FName str = *CurSessionName;
	CurSessionName = TEXT("");

	Sessions->EndSession(str);
	Sessions->DestroySession(str);
}

bool UUModGameInstance::StartNewGame(bool single, bool local, int32 max, FString map, FString hostName)
{
	ULocalPlayer* const Player = GetFirstGamePlayer();

	Player->PlayerController->ClientTravel("LoadScreen?game=" + AMenuGameMode::StaticClass()->GetPathName(), ETravelType::TRAVEL_Absolute);
	
	TSharedPtr<const FUniqueNetId> UserId = Player->GetPreferredUniqueNetId();

	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	IOnlineSessionPtr Sessions = NULL;

	if (OnlineSub == NULL) {
		return false;
	}

	Sessions = OnlineSub->GetSessionInterface();
	
	if (!Sessions.IsValid() || !UserId.IsValid()) {
		return false;
	}

	if (!CurSessionName.IsEmpty()){
		DestroyCurSession(Sessions);
		return false;
	}

	TSharedPtr<class FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
	
	if (single) {
		SessionSettings->bIsLANMatch = true;
		IsLAN = true;
	} else {
		SessionSettings->bIsLANMatch = local;
		IsLAN = local;
	}
	SessionSettings->bUsesPresence = true;
	if (single) {
		SessionSettings->NumPublicConnections = 1;
	} else {
		SessionSettings->NumPublicConnections = max;
	}	
	SessionSettings->NumPrivateConnections = 0;
	SessionSettings->bAllowInvites = true;
	SessionSettings->bAllowJoinInProgress = true;
	SessionSettings->bShouldAdvertise = true;
	SessionSettings->bAllowJoinViaPresence = true;
	SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

	SessionSettings->Set(SETTING_MAPNAME, map, EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings->Set(SETTING_GAMEMODE, AUModGameMode::StaticClass()->GetPathName(), EOnlineDataAdvertisementType::ViaOnlineService);	

	if (single) {
		hostName = TEXT("UMod_SinglePlayer");
	}
	
	CurConnectedIP = "127.0.0.1";
	CurConnectedAddress = "localhost";

	OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

	CurSessionMapName = FString("/Game/Maps/") + map;

	FName name = FName(*hostName);

	CurSessionName = hostName;

	return Sessions->CreateSession(*UserId, name, *SessionSettings);
}

FString ResolveIP(FString dns, FString &ResolvedIP)
{
	ISocketSubsystem* var = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
	ANSICHAR *t = TCHAR_TO_ANSI(*dns);
	FResolveInfo *inf = var->GetHostByName(t);
	while (!inf->IsComplete());

	if (inf->GetErrorCode() != 0) {
		ResolvedIP = FString("");
		return FString("ERROR_RESOLVE_FAILURE");
	}

	const FInternetAddr *addr = &inf->GetResolvedAddress();
	uint32 ip;
	addr->GetIp(ip);

	FString res = FString::Printf(TEXT("%d.%d.%d.%d"), 0xff & (ip >> 24), 0xff & (ip >> 16), 0xff & (ip >> 8), 0xff & ip);

	ResolvedIP = res;

	return FString("");
}

bool UUModGameInstance::PollServer(FString ip, int32 port, FString &error)
{
	FString WorkedIP;
	error = ResolveIP(ip, WorkedIP); //Resolve the IP address the user entered
	if (!error.IsEmpty()) {
		return false;
	}
	UE_LOG(UMod_Game, Warning, TEXT("IP Resolver result is : %s"), *WorkedIP);
	ULocalPlayer* const Player = GetFirstGamePlayer();
	FString str = WorkedIP + FString(":");
	str.AppendInt(port);
	GetGameEngine()->RunPollServer(str, Player);
	IsPollingServer = true;
	return true;
}

bool UUModGameInstance::JoinGame(FString ip, int32 port)
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	IOnlineSessionPtr Sessions = NULL;

	if (OnlineSub == NULL) {
		return false;
	}

	Sessions = OnlineSub->GetSessionInterface();

	if (!Sessions.IsValid()) {
		return false;
	}

	if (!CurSessionName.IsEmpty()){
		DestroyCurSession(Sessions);
		return false;
	}

	ULocalPlayer* const Player = GetFirstGamePlayer();

	//Run first checks
	if (Player->GetWorld() != NULL) {
		FString WorkedIP;
		FString Error = ResolveIP(ip, WorkedIP); //Resolve the IP address the user entered

		UE_LOG(UMod_Game, Warning, TEXT("IP Resolver result is : %s"), *WorkedIP);
		if (Error.IsEmpty()) { //Ok we can connect (IP is valid)
			FString str = WorkedIP + FString(":");
			str.AppendInt(port);
			ConnectIP = str;

			//Set vars for gloabl host ip/name retrieve
			CurConnectedIP = ConnectIP;
			CurConnectedAddress = ip + ":";
			CurConnectedAddress.AppendInt(port);
			//End

			Player->PlayerController->ClientTravel("LoadScreen?game=" + AMenuGameMode::StaticClass()->GetPathName(), ETravelType::TRAVEL_Absolute);
			
			DelayedServerConnect = true;
			return true;
		} else { //Don't connect : IP is not valid
			Disconnect(Error);
			return false;
		}
	} else { //Here would be a realy strange error
		Disconnect("ERROR_NULL_POINTER");
		return false;
	}
	//End
}

bool UUModGameInstance::ChangeGameResolution(FUModGameResolution res)
{
	if (GetGameResolution() == res) {
		return false;
	}

	if (res.GameWidth < MinResX || res.GameHeight < MinResY) {
		return false;
	}

	FDisplayMetrics metrics;
	FDisplayMetrics::GetDisplayMetrics(metrics);
	int32 width = metrics.PrimaryDisplayWidth;
	int32 height = metrics.PrimaryDisplayHeight;
	if (res.GameWidth > width || res.GameHeight > height) {
		return false;
	}
	const UGameViewportClient* Viewport = GetWorld()->GetGameViewport();
	UGameUserSettings* Settings = GEngine->GetGameUserSettings();
	EWindowMode::Type type;
	if (res.FullScreen) {
		type = EWindowMode::Fullscreen;
	} else {
		type = EWindowMode::Windowed;
	}
	Settings->SetScreenResolution(FIntPoint(res.GameWidth, res.GameHeight));
	Settings->SetFullscreenMode(type);
	Settings->RequestResolutionChange(res.GameWidth, res.GameHeight, type);
	Settings->ConfirmVideoMode();
	Settings->ApplyResolutionSettings(false);

	GConfig->SetInt(TEXT("Viewport"), TEXT("Width"), res.GameWidth, FPaths::GameConfigDir() + FString("UMod.Client.cfg"));
	GConfig->SetInt(TEXT("Viewport"), TEXT("Height"), res.GameHeight, FPaths::GameConfigDir() + FString("UMod.Client.cfg"));
	GConfig->SetBool(TEXT("Viewport"), TEXT("FullScreen"), res.FullScreen, FPaths::GameConfigDir() + FString("UMod.Client.cfg"));
	return true;
}

void UUModGameInstance::ShowFatalMessage(FString content)
{
	FString title;
	if (DedicatedStatic) {
		title = "UMod Server";
	} else {
		title = "UMod Client";
	}
	const FText* text = new FText(FText::FromString(title));
	FMessageDialog::Open(EAppMsgType::Type::Ok, FText::FromString(content), text);
	delete text;
	FGenericPlatformMisc::RequestExit(true);
}

FUModGameResolution UUModGameInstance::GetGameResolution()
{
	const UGameViewportClient* Viewport = GetWorld()->GetGameViewport();
	if (Viewport == NULL) {
		return FUModGameResolution();
	}
	FVector2D vec;
	Viewport->GetViewportSize(vec);
	return FUModGameResolution(vec.X, vec.Y, Viewport->IsFullScreenViewport());
}

TArray<FUModGameResolution> UUModGameInstance::GetAvailableGameResolutions()
{
	FScreenResolutionArray Resolutions;
	if (RHIGetAvailableResolutions(Resolutions, false)) {
		TArray<FUModGameResolution> list;
		for (const FScreenResolutionRHI& EachResolution : Resolutions) {
			if (EachResolution.Width >= MinResX && EachResolution.Height >= MinResY) {
				list.Add(FUModGameResolution(EachResolution.Width, EachResolution.Height, false));
			}
		}
		return list;
	} else {
		UE_LOG(UMod_Game, Error, TEXT("Screen Resolutions could not be obtained"));
		return TArray<FUModGameResolution>();
	}
}

void UUModGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnCreateSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub != NULL) {
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

				Sessions->StartSession(SessionName);
			} else {
				Disconnect("Unable to create session : OnCreateSessionComplete.bWasSuccessful is false !");
			}
		}
	}
}

void UUModGameInstance::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnStartSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}
	}
	
	if (bWasSuccessful)
	{
		DelayedRunMap = true;
	} else {
		Disconnect("Unable to start session : OnStartOnlineGameComplete.bWasSuccessful is false !");
	}
}

/**
 * This function is intended to prevent user connecting to itself (yes indeed UE4 loves to connect to maps that are internal)
 * Note : Epic Games seriously you realy should think of making a better network system. I like your Engine I find realy usefull for easy rendering/physics but again please FIIIIIXXXXX network !
 * PS : I'm realy tired to have to come behind fixing your network issues ! Anyways I still find your Engine better than Unity (obviously Unity has a much more broken network so...)
 */
void CreateNetworkHackerActor()
{

}

void UUModGameInstance::OnAsyncLevelLoadingDone()
{
	if (IsLAN) {
		FString opt = FString("game=") + AUModGameMode::StaticClass()->GetPathName() + FString("?listen?bIsLanMatch");
		UGameplayStatics::OpenLevel(GetWorld(), FName(*CurSessionMapName), true, opt);
	} else {
		FString opt = FString("game=") + AUModGameMode::StaticClass()->GetPathName() + FString("?listen");
		UGameplayStatics::OpenLevel(GetWorld(), FName(*CurSessionMapName), true, opt);
	}
	IsLAN = false;
	CurSessionMapName = NULL;
	DelayedRunMap = false;

	IsListen = true;
	BrokeListenServer = false;
}

uint32 ticks = 0;
bool BrokeDedicatedServer = false;
void UUModGameInstance::Tick(float DeltaTime)
{
	//Redirect FNetworkNorify to this instance
	if (IsListen && !BrokeListenServer) { //We are a listen server
		UWorld *World = GetWorld();
		if (World != NULL && World->GetNetDriver() != NULL) {
			Notify = World->GetNetDriver()->Notify;
			World->GetNetDriver()->Notify = this;
			UE_LOG(UMod_Game, Error, TEXT("HAVE FUN UWORLD ! I DISCONNECTED YOU FROM THE NETWORK !"));
			BrokeListenServer = true;
		}
	}
	if (IsDedicated && !BrokeDedicatedServer) { //We are a dedicated server
		UWorld *World = GetWorld();
		if (World != NULL && World->GetNetDriver() != NULL) {
			Notify = World->GetNetDriver()->Notify;
			World->GetNetDriver()->Notify = this;
			UE_LOG(UMod_Game, Error, TEXT("HAVE FUN UWORLD ! I DISCONNECTED YOU FROM THE NETWORK !"));
			BrokeDedicatedServer = true;
		}
	}
	//End

	if (DelayedRunMap) {
		ticks++;
		if (ticks >= 5) {
			if (IsLAN) {
				//FString opt = FString("game=") + AUModGameMode::StaticClass()->GetPathName() + FString("?listen?bIsLanMatch");
				//UGameplayStatics::OpenLevel(GetWorld(), FName(*CurSessionMapName), true, opt);
				FLatentActionInfo inf;
				inf.CallbackTarget = this;
				inf.ExecutionFunction = "OnAsyncLevelLoadingDone";
				inf.UUID = 0;
				inf.Linkage = 0;
				UGameplayStatics::LoadStreamLevel(GetWorld(), FName(*CurSessionMapName), false, false, inf);
			} else {
				//FString opt = FString("game=") + AUModGameMode::StaticClass()->GetPathName() + FString("?listen");
				//UGameplayStatics::OpenLevel(GetWorld(), FName(*CurSessionMapName), true, opt);
				FLatentActionInfo inf;
				inf.CallbackTarget = this;
				inf.ExecutionFunction = FName("OnAsyncLevelLoadingDone");
				UGameplayStatics::LoadStreamLevel(GetWorld(), FName(*CurSessionMapName), false, false, inf);
			}

			/*IsLAN = false;
			CurSessionMapName = NULL;
			DelayedRunMap = false;*/
			ticks = 0;
		}		
	} else if (DelayedServerConnect) {
		ticks++;
		if (ticks >= 5) {
			ULocalPlayer* const Player = GetFirstGamePlayer();
			
			//I would like to also add UGameplayStatics::LoadStreamLevel however I don't know the server map that will be used... To know that I need a preconnection system...
			OnNetworkConnectionCreation(Player);
			
			ticks = 0;			
			DelayedServerConnect = false;
		}
	}

	if (!IsDedicated && !IsDisplayCreated) {
		const UGameViewportClient* Viewport = GetWorld()->GetGameViewport();
		if (Viewport != NULL) {
			OnDisplayCreated();
			IsDisplayCreated = true;
		}
	}

	AssetsManager->UpdateTick();

	/*if (!CurSessionName.IsEmpty()) {
		IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
		IOnlineSessionPtr Sessions = NULL;
		if (OnlineSub == NULL) {
			return;
		}
		Sessions = OnlineSub->GetSessionInterface();
		if (!Sessions.IsValid()) {
			return;
		}

		EOnlineSessionState::Type state = Sessions->GetSessionState(*CurSessionName);
		if (state == EOnlineSessionState::Ended) {
			Disconnect(FString("Lost Connection..."));
		}
	}*/
}

bool UUModGameInstance::IsTickable() const
{
	return true;
}

TStatId UUModGameInstance::GetStatId() const
{
	return Super::GetStatID();
}

FString netError;
int32 cur;
int32 total;
int32 status;
void UUModGameInstance::Disconnect(FString error) //TODO : Make something to fix the fucking dedicated server CRASHING OVER AND OVER by calling this function !
{
	if (!ConnectIP.IsEmpty()) {
		ULocalPlayer * const ply = GetFirstGamePlayer();
		if (ply->GetWorld() != NULL && ply->GetWorld()->GetNetDriver() != NULL) {
			ply->GetWorld()->GetNetDriver()->Shutdown();
		}
		ConnectIP = "";

		netError = error;
		ULocalPlayer* const Player = GetFirstGamePlayer();
		Player->PlayerController->ClientTravel("LoadScreen?game=" + AMenuGameMode::StaticClass()->GetPathName(), ETravelType::TRAVEL_Absolute);
		return;
	}

	if (IsListen) {
		IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
		IOnlineSessionPtr Sessions = NULL;
		if (OnlineSub == NULL) {
			return;
		}
		Sessions = OnlineSub->GetSessionInterface();
		if (!Sessions.IsValid()) {
			return;
		}
		if (!CurSessionName.IsEmpty()) {
			DestroyCurSession(Sessions);
		}
		IsListen = false;
	}

	AssetsManager->HandleServerDisconnect();

	//Set gloabl host ip/address vars
	CurConnectedIP = FString();
	CurConnectedAddress = FString();
	//End

	netError = error;
	ULocalPlayer* const Player = GetFirstGamePlayer();	
	Player->PlayerController->ClientTravel("LoadScreen?game=" + AMenuGameMode::StaticClass()->GetPathName(), ETravelType::TRAVEL_Absolute);
}
void UUModGameInstance::SetLoadData(int32 total1, int32 cur1, int32 status1)
{
	total = total1;
	cur = cur1;
	status = status1;
}
FString UUModGameInstance::GetNetErrorMessage()
{
	return netError;
}
int32 UUModGameInstance::GetNumObjectToLoad()
{
	return total;
}
int32 UUModGameInstance::GetCurLoadedObjectNum()
{
	return cur;
}
int32 UUModGameInstance::GetNetLoadStatus()
{
	return status;
}

void UUModGameInstance::ReturnToMainMenu()
{
	netError = TEXT("");
	ULocalPlayer* const Player = GetFirstGamePlayer();
	Player->PlayerController->ClientTravel("MainMenu?game=" + AMenuGameMode::StaticClass()->GetPathName(), ETravelType::TRAVEL_Absolute);
}

AUModCharacter* UUModGameInstance::GetLocalPlayer()
{
	ULocalPlayer* const Player = GetFirstGamePlayer();
	ACharacter *c = Player->PlayerController->GetCharacter();
	if (c != NULL) {
		return Cast<AUModCharacter>(c);
	}
	return NULL;
}

FString UUModGameInstance::GetHostIP()
{
	return CurConnectedIP;
}

FString UUModGameInstance::GetHostAddress()
{
	return CurConnectedAddress;
}

FString UUModGameInstance::GetHostName()
{
	return HostName;
}

FString UUModGameInstance::GetGameMode()
{
	return GameMode;
}

bool UUModGameInstance::IsListenServer()
{
	return IsListen;
}

void UUModGameInstance::ReloadLua()
{
	delete Lua;
	Lua = new LuaEngine(this);
}

bool UUModGameInstance::IsEditor()
{
#if WITH_EDITOR
	return GIsEditor;
#else
	return false;
#endif
}

void UUModGameInstance::ExitGame()
{
#if WITH_EDITOR
	if (GIsEditor) {
		return;
	}
	FPlatformMisc::RequestExit(false);
#else
	FPlatformMisc::RequestExit(false);
#endif
}