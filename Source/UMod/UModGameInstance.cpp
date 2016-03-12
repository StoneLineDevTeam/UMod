// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "UModGameInstance.h"

#include "Player/UModPlayerState.h"

#include "Game/UModGameMode.h"
#include "Game/MenuGameMode.h"

#include "Console/ConsoleDestroyer.h"
#include "Console/UModConsoleManager.h"

const FString GVersion = FString("0.4 - Alpha");
FString LuaVersion;
const FString LuaEngineVersion = FString("NULL");
static bool DedicatedStatic;

const uint32 MinResX = 1200;
const uint32 MinResY = 650;

UUModGameInstance::UUModGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UUModGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UUModGameInstance::OnStartOnlineGameComplete);

	AssetsManager = NewObject<UUModAssetsManager>();
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

void UUModGameInstance::OnNetworkFailure(UWorld *world, UNetDriver *driver, ENetworkFailure::Type failType, const FString &ErrorMessage)
{
	UE_LOG(UMod_Game, Error, TEXT("Network error occured !"));

	FString err = ErrorMessage;
	Disconnect(err);	
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


	//Found a way to set port !
	int32 ServerPort = 0;
	GConfig->GetInt(TEXT("Common"), TEXT("Port"), ServerPort, FPaths::GameConfigDir() + FString("UMod.Server.cfg"));
	if (ServerPort == 0) {
		ServerPort = 25565;
		GConfig->SetInt(TEXT("Common"), TEXT("Port"), ServerPort, FPaths::GameConfigDir() + FString("UMod.Server.cfg"));
	}
	GConfig->SetInt(TEXT("URL"), TEXT("Port"), ServerPort, GEngineIni);


	UE_LOG(UMod_Game, Log, TEXT("UMod - Starting Lua Engine..."));
	//Lua = new LuaInterface(this);
	Lua = new LuaEngine(this);
	LuaVersion = Lua->GetLuaVersion();
	FString lua = FString("LuaEngine V.") + LuaEngineVersion + FString(" | Lua V.") + LuaVersion;
	UE_LOG(UMod_Lua, Log, TEXT("%s"), *lua);

	Lua->RunScript(FPaths::GameDir() + FString("UMod.lua"));
	Lua->RunScriptFunction(ETableType::GAMEMODE, 0, 1, "Initialize", 's', lua);

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
	FGenericPlatformMisc::RequestExit(false);
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

uint32 ticks = 0;
void UUModGameInstance::Tick(float DeltaTime)
{	
	if (DelayedRunMap) {
		ticks++;
		if (ticks >= 5) {
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
			ticks = 0;
		}		
	} else if (DelayedServerConnect) {
		ticks++;
		if (ticks >= 5) {
			ULocalPlayer* const Player = GetFirstGamePlayer();
						
			Player->PlayerController->ClientTravel(ConnectIP, ETravelType::TRAVEL_Absolute);
			AUModPlayerState *state = Cast<AUModPlayerState>(Player->PlayerController->PlayerState);
			if (state == NULL) {
				Disconnect(FString("An unexpected error has occured."));
				return;
			}
			state->InitPlayerConnection(256);
			
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
void UUModGameInstance::Disconnect(FString error)
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

	AssetsManager->HandleServerDisconnect();

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