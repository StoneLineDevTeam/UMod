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

#include "Renderer/Render2D.h"

#include "IpNetDriver.h"
#include "IpConnection.h"


FString LuaVersion;

bool UUModGameInstance::DedicatedStatic = false;
bool UUModGameInstance::ShouldRunCMD = false;
FString UUModGameInstance::RunCMD = "";

UUModGameInstance::UUModGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UUModGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UUModGameInstance::OnStartOnlineGameComplete);		
}

//Network hack
void UUModGameInstance::OnNetworkConnectionCreation(ULocalPlayer *Player)
{
	//Yeah no longer need for hacky thing as now UUModGameEngine actualy does this stuff inside Browse
	Player->PlayerController->ClientTravel(ConnectIP, ETravelType::TRAVEL_Absolute);
}
//End

void UUModGameInstance::SetupClientConnection(FString GM, FString HN, uint8 Flags)
{
	GameMode = GM;
	HostName = HN;
	//TODO : maybe use flags...
}

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
	FEngineVersion vers = FEngineVersion::Current();
	FString version = FString::FromInt(vers.GetMajor()) + "." + FString::FromInt(vers.GetMinor()) + "." + FString::FromInt(vers.GetPatch());	

	return version;
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
	if (UUModGameEngine::IsPollingServer) {
		//We are obviously no longer polling the server (we crashed !)
		UUModGameEngine::IsPollingServer = false;
		return;
	}
	if (!UUModGameEngine::IsPollingServer && !DelayedServerConnect && !UUModGameEngine::IsDedicated) {
		UE_LOG(UMod_Game, Error, TEXT("Network error occured !"));

		FString err = ErrorMessage;
		Disconnect(err);		
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
	if (IsEditor()) {		
		UE_LOG(UMod_Game, Error, TEXT("Aborting game load : PIE detected !"));
		return;
	}
	//Retrieve the AssetsManager from UModGameEngine
	AssetsManager = GetGameEngine()->AssetsManager;

	//Console log retriever hack
	ConsoleManager = NewObject<UUModConsoleManager>();
	ConsoleManager->Game = this;
	GLog->AddOutputDevice(ConsoleManager);
	GLog->SerializeBacklog(ConsoleManager);
	GLog->EnableBacklog(true);
	//End
		
	UE_LOG(UMod_Game, Log, TEXT("UMod - V.%s | Engine V.%s"), *GetGameVersion(), *GetEngineVersion());

	GEngine->ConsoleClass = UConsoleDestroyer::StaticClass();

	GEngine->NetworkFailureEvent.AddUObject(this, &UUModGameInstance::OnNetworkFailure);
	GEngine->TravelFailureEvent.AddUObject(this, &UUModGameInstance::OnTravelFailure);

	/*Hack NET vars*/
	//Found a way to set port !
	int32 ServerPort = 0;
	GConfig->GetInt(TEXT("Common"), TEXT("Port"), ServerPort, ServerCFG);
	if (ServerPort == 0) {
		ServerPort = 25565;
		GConfig->SetInt(TEXT("Common"), TEXT("Port"), ServerPort, ServerCFG);
	}
	GConfig->SetInt(TEXT("URL"), TEXT("Port"), ServerPort, GEngineIni);
	//Timeout variables
	ConnectTimeout = 0;
	Timeout = 0;
	ServerTimeout = 0;
	GConfig->GetFloat(TEXT("Common"), TEXT("ConnectTimeout"), ConnectTimeout, ServerCFG);
	GConfig->GetFloat(TEXT("Common"), TEXT("Timeout"), Timeout, ServerCFG);
	GConfig->GetFloat(TEXT("Common"), TEXT("ServerTimeout"), ServerTimeout, ServerCFG);
	if (ConnectTimeout == 0) {
		ConnectTimeout = 5;
		GConfig->SetFloat(TEXT("Common"), TEXT("ConnectTimeout"), ConnectTimeout, ServerCFG);
	}
	if (Timeout == 0) {
		Timeout = 45;
		GConfig->SetFloat(TEXT("Common"), TEXT("Timeout"), Timeout, ServerCFG);
	}
	if (ServerTimeout == 0) {
		ServerTimeout = 10;
		GConfig->SetFloat(TEXT("Common"), TEXT("ServerTimeout"), Timeout, ServerCFG);
	}
	//End
	//Retrieve host name
	GConfig->GetString(TEXT("Common"), TEXT("HostName"), HostName, ServerCFG);
	if (HostName.IsEmpty()) {
		HostName = "An UMod Server";
		GConfig->SetString(TEXT("Common"), TEXT("HostName"), *HostName, ServerCFG);
	}
	//Retrieve lua GameMode
	GConfig->GetString(TEXT("Common"), TEXT("GameMode"), GameMode, ServerCFG);
	if (GameMode.IsEmpty()) {
		GameMode = FString("Sandbox");
		GConfig->SetString(TEXT("Common"), TEXT("GameMode"), *GameMode, ServerCFG);
	}
	/*End*/
	
	UE_LOG(UMod_Game, Log, TEXT("UMod - Starting Lua Engine..."));
	Lua = new LuaEngine(this);
	if (!IsEditor()) {
		LuaVersion = Lua->GetLuaVersion();
		FString lua = FString("LuaEngine V.") + LuaEngineVersion + FString(" | Lua V.") + LuaVersion;
		UE_LOG(UMod_Lua, Log, TEXT("%s"), *lua);

		Lua->RunScript(FPaths::GameDir() + FString("UMod.lua"));
		//Lua->RunScriptFunctionTwoParam<int, FString>(ETableType::GAMEMODE, 0, "Initialize", FLuaParam<int>(25), FLuaParam<FString>(lua));
	}
	
	if (!IsDedicatedServer()) {
		UE_LOG(UMod_Game, Log, TEXT("Precaching required assets..."));
		float cur = FPlatformTime::ToMilliseconds(FPlatformTime::Cycles());
		//Sending precache request for all surface types
		EUModSurfaceTypes::FRegisterSurfaceTypes();
		for (TPair<FString, FSurfaceType*> Elem : UModSurfaceTypes) {
			UE_LOG(UMod_Game, Log, TEXT("Precaching SurfaceType : %s"), *Elem.Key);
			Elem.Value->Precache();
		}		

		//Sending global precache request
		UUModAssetsManager::PrecacheAssets.Broadcast();
		float elapsed = (FPlatformTime::ToMilliseconds(FPlatformTime::Cycles()) - cur) / 1000.0F;
		UE_LOG(UMod_Game, Log, TEXT("Done (%f seconds) !"), elapsed);
	}
}

bool UUModGameInstance::IsDedicatedServer()
{
	return UUModGameEngine::IsDedicated;
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

void UUModGameInstance::ShowMessage(FString content)
{
	FString title;
	if (DedicatedStatic) {
		title = "UMod Server";
	}
	else {
		title = "UMod Client";
	}
	const FText* text = new FText(FText::FromString(title));
	FMessageDialog::Open(EAppMsgType::Type::Ok, FText::FromString(content), text);
	delete text;
}

void UUModGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("OnCreateSessionComplete %s, %d"), *SessionName.ToString(), bWasSuccessful));

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub != NULL) {
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid()) {
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			if (bWasSuccessful) {
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
	if (OnlineSub) {
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid()) {
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}
	}
	
	if (bWasSuccessful) {
		DelayedRunMap = true;
	} else {
		Disconnect("Unable to start session : OnStartOnlineGameComplete.bWasSuccessful is false !");
	}
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

	UUModGameEngine::IsListen = true;
}

uint32 ticks = 0;
bool BrokeDedicatedServer = false;
void UUModGameInstance::Tick(float DeltaTime)
{
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

	if (DedicatedStatic && ShouldRunCMD && ConsoleManager != NULL) {
		ConsoleManager->RunConsoleCommand(RunCMD);
		ShouldRunCMD = false;
		RunCMD = "";
	}

	if (!DedicatedStatic && !IsEditor() && !CurConnectedIP.IsEmpty()) {
		//Check if any connection issues		
		if (GetWorld() != NULL) {
			UNetDriver *Net = GetWorld()->GetNetDriver();
			if (Net != NULL) {
				if ((Net->Time - Net->ServerConnection->LastReceiveTime) > 2) { //Over 2 seconds no packets received
					Net->InitialConnectTimeout = Timeout;
					Net->ConnectionTimeout = Timeout;
					IsConnectionHealthy = false;
					InTimeOut = Net->InitialConnectTimeout - (Net->Time - Net->ServerConnection->LastReceiveTime);
				} else {
					IsConnectionHealthy = true;
					InTimeOut = 0;
				}
			}
		}
	}

	AssetsManager->UpdateTick();
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
void UUModGameInstance::Disconnect(FString error) //TODO : Make something to fix the fucking dedicated server CRASHING OVER AND OVER by calling this function !
{
	if (!ConnectIP.IsEmpty()) {
		ConnectIP = "";
	}

	if (UUModGameEngine::IsListen) {
		IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
		IOnlineSessionPtr Sessions = NULL;
		if (OnlineSub != NULL) {
			
			if (Sessions.IsValid()) {
				Sessions = OnlineSub->GetSessionInterface();
				if (!CurSessionName.IsEmpty()) {
					DestroyCurSession(Sessions);
				}
			}
		}
		UUModGameEngine::IsListen = false;
	}
	
	GetGameEngine()->NetworkCleanUp();

	AssetsManager->HandleServerDisconnect();

	//Set gloabl host ip/address vars
	CurConnectedIP.Empty();
	CurConnectedAddress.Empty();
	//End

	netError = error;
	ULocalPlayer* const Player = GetFirstGamePlayer();	
	Player->PlayerController->ClientTravel("LoadScreen?game=" + AMenuGameMode::StaticClass()->GetPathName(), ETravelType::TRAVEL_Absolute);	
}
FString UUModGameInstance::GetNetErrorMessage()
{
	return netError;
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

FConnectionStats UUModGameInstance::GetConnectionInfo()
{
	FConnectionStats stats;
	stats.HostIP = CurConnectedIP;
	stats.HostAddress = CurConnectedAddress;
	stats.HostName = ConsoleManager->GetConsoleVar<FString>("HostName");
	stats.ConnectionProblem = !IsConnectionHealthy;
	stats.SecsBeforeDisconnect = InTimeOut;
	stats.GameMode = GameMode;
	return stats;
}

/*Begin server side library*/
FString UUModGameInstance::GetHostName()
{
	return HostName;
}
FString UUModGameInstance::GetGameMode()
{
	return GameMode;
}
/*End*/

bool UUModGameInstance::IsListenServer()
{
	return UUModGameEngine::IsListen;
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