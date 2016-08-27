#include "UMod.h"
#include "UModGameEngine.h"
#include "Engine/PendingNetGame.h"
#include "Console/UModConsoleManager.h"
#include "NetCore/ServerHandler.h"
#include "NetCore/ClientHandler.h"

#include "UModAssetsManager.h"

/* Console system (Platform Dependent) */
//EDIT 18/08/2016 : Restructured UMod OS specific functions
/*#if PLATFORM_WINDOWS
#include "Console/Win32ConsoleHandler.h"
#endif*/
/* End */

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
IMPLEMENT_CONTROL_CHANNEL_MESSAGE(UModChangeMap);
//The poll control channel message
IMPLEMENT_CONTROL_CHANNEL_MESSAGE(UModPoll);

const uint32 MinResX = 1200;
const uint32 MinResY = 650;

bool UUModGameEngine::IsDedicated = false;
bool UUModGameEngine::IsListen = false;
bool UUModGameEngine::IsPollingServer = false;

bool SendPollPacket;

EBrowseReturnVal::Type UUModGameEngine::Browse(FWorldContext& WorldContext, FURL URL, FString& Error)
{
	UE_LOG(UMod_Game, Log, TEXT("[DEBUG]New travel URL : %s"), *URL.ToString())
	if (URL.IsInternal() && GIsClient && !URL.IsLocalInternal()) {
		//Init network
		EBrowseReturnVal::Type t = Super::Browse(WorldContext, URL, Error);
		//Hack network now (yeah fuck you UE4)
		if (t == EBrowseReturnVal::Type::Success || t == EBrowseReturnVal::Type::Pending) {
			UNetDriver *NetDriver = GEngine->FindNamedNetDriver(WorldContext.PendingNetGame, NAME_PendingNetDriver);
			NetDriver->InitialConnectTimeout = GetGame()->ConnectTimeout;
			NetDriver->ConnectionTimeout = GetGame()->ConnectTimeout;
			if (NetHandleCL != NULL) { //FIX : delete causing weird access violation when switching map
				UE_LOG(UMod_Game, Log, TEXT("Network Handler Client removal..."));
				delete NetHandleCL;
			}
			//HEHEHEHHE ! You did not saw that, you are so stupid UE4 !
			NetHandleCL = new UClientHandler();
			NetHandleCL->InitHandler(this, NetDriver->Notify, SendPollPacket);
			NetDriver->Notify = NetHandleCL;
			SendPollPacket = false;
			UE_LOG(UMod_Game, Error, TEXT("Disconnected UPendingNetGame from network notify system !"));
		}
		return t;
	}
	if (IsDedicated || IsListen) {
		//Break server (that will be much more reliable)
		EBrowseReturnVal::Type t = Super::Browse(WorldContext, URL, Error);
		UNetDriver *NetDriver = GetGameWorld()->GetNetDriver();
		NetHandleSV = new UServerHandler();
		NetHandleSV->InitHandler(this, NetDriver->Notify);
		NetDriver->Notify = NetHandleSV;
		NetDriver->InitialConnectTimeout = GetGame()->ServerTimeout;
		NetDriver->ConnectionTimeout = GetGame()->ServerTimeout;
		UE_LOG(UMod_Game, Error, TEXT("Disconnected UWorld from network notify system !"));
		return t;
	}
	//Ok relaying on the normal browse system as we don't want to change UE4's map loading system
	return Super::Browse(WorldContext, URL, Error);
}

bool UUModGameEngine::LoadMap(FWorldContext & WorldContext, FURL URL, class UPendingNetGame * Pending, FString & Error)
{
	bool b = Super::LoadMap(WorldContext, URL, Pending, Error);
	if (NetHandleCL != NULL) { //We are on a client that is connected to a server : deconstruct notify system		
		UNetDriver *NetDriver = GEngine->FindNamedNetDriver(WorldContext.World(), NAME_GameNetDriver);
		if (NetDriver == NULL) {
			UE_LOG(UMod_Game, Error, TEXT("Unable to retrieve GameNetDriver, client may be unstable..."));
		} else {
			NetHandleCL->InitHandler(this, NetDriver->Notify, false);
			NetDriver->Notify = NetHandleCL;
		}		
	}
	return b;
}

void UUModGameEngine::NetworkCleanUp()
{
	if (GetGameWorld()->GetNetDriver() != NULL) {
		GetGameWorld()->GetNetDriver()->Shutdown();
	}	
	if (NetHandleSV != NULL) {
		delete NetHandleSV;
	}
}

UUModGameInstance *UUModGameEngine::GetGame()
{
	return Cast<UUModGameInstance>(GameInstance);
}

void UUModGameEngine::Init(class IEngineLoop* InEngineLoop)
{
	AssetsManager = new UUModAssetsManager();
	UUModAssetsManager::Instance = AssetsManager;

	//YOUHOU ! It's overWRITE now not override !
#if UE_BUILD_SERVER
	FString MapToLoad;
	GConfig->GetString(TEXT("Dedicated"), TEXT("Map"), MapToLoad, ServerCFG);
	if (MapToLoad.IsEmpty()) {
		MapToLoad = FString("FirstPersonExampleMap");
		GConfig->SetString(TEXT("Dedicated"), TEXT("Map"), *MapToLoad, ServerCFG);
	}

	//Check if we want logging
	bool Log;
	GConfig->GetBool(TEXT("Common"), TEXT("DoLogging"), Log, ServerCFG);
	GConfig->SetBool(TEXT("Common"), TEXT("DoLogging"), Log, ServerCFG);

	FString mapPath;
	EResolverResult res = AssetsManager->ResolveAsset(MapToLoad, EUModAssetType::MAP, mapPath);
	if (res != EResolverResult::SUCCESS) {
		UUModGameInstance::ShowMessage("AssetsManager->ResolveAsset returned " + AssetsManager->GetErrorMessage(res));
		UE_LOG(UMod_Maps, Error, TEXT("Aborting engine load : %s"), *AssetsManager->GetErrorMessage(res));
		return;
	}
	const UGameMapsSettings* GameMapsSettings = GetDefault<UGameMapsSettings>();
	GameMapsSettings->SetGameDefaultMap(mapPath);
	GameMapsSettings->SetGlobalDefaultGameMode("UModGameMode");

	GLogConsole->Show(true);
	GLogConsole->SetSuppressEventTag(false);

	FString str = mapPath + " -log";
	FCommandLine::Set(*str);

	IsDedicated = true;
	IsListen = false;
#else
	FString str = FCommandLine::Get();
	TArray<FString> cmds;
	str.ParseIntoArray(cmds, TEXT(" "));
	if (cmds.Contains("-server")) {		
		IsDedicated = true;
		UUModGameInstance::DedicatedStatic = true;

		//Retrieve map name from config
		FString MapToLoad;
		GConfig->GetString(TEXT("Dedicated"), TEXT("Map"), MapToLoad, ServerCFG);
		if (MapToLoad.IsEmpty()) {
			MapToLoad = FString("UMod:FirstPersonExampleMap");
			GConfig->SetString(TEXT("Dedicated"), TEXT("Map"), *MapToLoad, ServerCFG);
		}

		//Check if we want logging
		bool Log;
		GConfig->GetBool(TEXT("Common"), TEXT("DoLogging"), Log, ServerCFG);
		GConfig->SetBool(TEXT("Common"), TEXT("DoLogging"), Log, ServerCFG);
				
		FString mapPath;
		EResolverResult res = AssetsManager->ResolveAsset(MapToLoad, EUModAssetType::MAP, mapPath);
		if (res != EResolverResult::SUCCESS) {
			UUModGameInstance::ShowMessage("AssetsManager->ResolveAsset returned " + AssetsManager->GetErrorMessage(res));
			UE_LOG(UMod_Maps, Error, TEXT("Aborting engine load : %s"), *AssetsManager->GetErrorMessage(res));
			return;
		}
		const UGameMapsSettings* GameMapsSettings = GetDefault<UGameMapsSettings>();
		GameMapsSettings->SetGameDefaultMap(mapPath);
		GameMapsSettings->SetGlobalDefaultGameMode("Game/UModGameMode.h");

		GLogConsole->Show(true);
		GLogConsole->SetSuppressEventTag(false);
		
		FString str = mapPath + " -server";
		if (Log) {
			str.Append(" -log");
		}
		FCommandLine::Set(*str);
	} else {
		IsDedicated = false;
		UUModGameInstance::DedicatedStatic = false;

		bool Log;
		GConfig->GetBool(TEXT("Common"), TEXT("DoLogging"), Log, ClientCFG);
		GConfig->SetBool(TEXT("Common"), TEXT("DoLogging"), Log, ClientCFG);
		FString str = "/Game/Internal/Maps/MainMenu -game";
		if (Log) {
			str.Append(" -log");
		}
		FCommandLine::Set(*str);
	}
#endif

	FUModPlatformUtils Platform = FUModPlatformUtils();
	FString path = FPaths::GameDir() + "/UMod.ico";
	void* icon = Platform.LoadIconFromFile(path, 32, 32);
	if (!IsDedicated) {
		TSharedPtr<SWindow> WindowPtr = GameViewportWindow.Pin();
		WindowPtr->SetSizingRule(ESizingRule::FixedSize);
		WindowPtr->SetTitle(FText::FromString("UMod Client"));
		Platform.SetWindowIcon(WindowPtr->GetNativeWindow()->GetOSWindowHandle(), icon);
		Platform.DisableWindowResize(WindowPtr->GetNativeWindow()->GetOSWindowHandle());		
		WindowPtr->HideWindow();
		Super::Init(InEngineLoop);
		OnDisplayCreated();
		UE_LOG(UMod_Game, Log, TEXT("Display data has been overwritten."));
		UE_LOG(UMod_Game, Log, TEXT("Running UMod as client application..."));
	} else {
		UE_LOG(UMod_Game, Log, TEXT("Running UMod as server application..."));		
		/* Console system (Platform Dependent) */		
		void* hdl = Platform.GetConsoleHandle();		
		Platform.SetConsoleIcon(hdl, icon);
		Platform.SetConsoleTitleString(hdl, "UMod Dedicated Server");
		Platform.BindStdIO(hdl);
		FSTDInputThread::StartThread();
		/* End */
		Super::Init(InEngineLoop);
	}
}

/*void UUModGameEngine::Tick(float DeltaSeconds, bool bIdleMode)
{
	Super::Tick(DeltaSeconds, bIdleMode);
	//Super hacky code I know but UE4 decided that it was great to ReshapeWindow in tick I had to fuck it up...
	//In other words we were not in aggreement...
	TSharedPtr<SWindow> WindowPtr = GameViewportWindow.Pin();
	if (WindowPtr.IsValid()) {
		FVector2D vec = WindowPtr->GetSizeInScreen();
		if (vec.X != CurrentGameResolution.GameWidth || vec.Y != CurrentGameResolution.GameHeight) {
			OnDisplayCreated();
		}
	}
}*/

void UUModGameEngine::OnDisplayCreated()
{
	int32 width = MinResX;
	int32 height = MinResY;

	FDisplayMetrics metrics;
	FDisplayMetrics::GetDisplayMetrics(metrics);
	int32 PW = metrics.PrimaryDisplayWidth;
	int32 PH = metrics.PrimaryDisplayHeight;

	bool full;
	GConfig->GetInt(TEXT("Viewport"), TEXT("Width"), width, ClientCFG);
	GConfig->GetInt(TEXT("Viewport"), TEXT("Height"), height, ClientCFG);
	GConfig->GetBool(TEXT("Viewport"), TEXT("FullScreen"), full, ClientCFG);
	GSystemResolution.ResX = width;
	GSystemResolution.ResY = height;
	if (full) {
		GSystemResolution.WindowMode = EWindowMode::Fullscreen;
	} else {
		GSystemResolution.WindowMode = EWindowMode::Windowed;
	}
	//UGameUserSettings::RequestResolutionChange(GSystemResolution.ResX, GSystemResolution.ResY, GSystemResolution.WindowMode, false);
	TSharedPtr<SWindow> WindowPtr = GameViewportWindow.Pin();	
	WindowPtr->ReshapeWindow(FVector2D(PW / 2 - width / 2, PH / 2 - height / 2), FVector2D(GSystemResolution.ResX, GSystemResolution.ResY));
	WindowPtr->SetWindowMode(GSystemResolution.WindowMode);
	WindowPtr->ShowWindow();
	WindowPtr->ReshapeWindow(FVector2D(PW / 2 - width / 2, PH / 2 - height / 2), FVector2D(GSystemResolution.ResX, GSystemResolution.ResY));
	CurrentGameResolution = FUModGameResolution(GSystemResolution.ResX, GSystemResolution.ResY, full);
}

bool UUModGameEngine::ChangeGameResolution(FUModGameResolution res)
{
	//Use ReshapeWindow is SWindow in order to change game resolution (it works better)
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

	GConfig->SetInt(TEXT("Viewport"), TEXT("Width"), res.GameWidth, ClientCFG);
	GConfig->SetInt(TEXT("Viewport"), TEXT("Height"), res.GameHeight, ClientCFG);
	GConfig->SetBool(TEXT("Viewport"), TEXT("FullScreen"), res.FullScreen, ClientCFG);

	GSystemResolution.ResX = res.GameWidth;
	GSystemResolution.ResY = res.GameHeight;
	if (res.FullScreen) {
		GSystemResolution.WindowMode = EWindowMode::Fullscreen;
	}
	else {
		GSystemResolution.WindowMode = EWindowMode::Windowed;
	}
	//UGameUserSettings::RequestResolutionChange(GSystemResolution.ResX, GSystemResolution.ResY, GSystemResolution.WindowMode, false);
	TSharedPtr<SWindow> WindowPtr = GameViewportWindow.Pin();	
	WindowPtr->ReshapeWindow(WindowPtr->GetPositionInScreen(), FVector2D(GSystemResolution.ResX, GSystemResolution.ResY));
	WindowPtr->SetWindowMode(GSystemResolution.WindowMode);
	CurrentGameResolution = res;
	return true;
}

FUModGameResolution UUModGameEngine::GetGameResolution()
{
	return CurrentGameResolution;
}

TArray<FUModGameResolution> UUModGameEngine::GetAvailableGameResolutions()
{
	FDisplayMetrics metrics;
	FDisplayMetrics::GetDisplayMetrics(metrics);
	int32 width = metrics.PrimaryDisplayWidth;
	int32 height = metrics.PrimaryDisplayHeight;
	TArray<FUModGameResolution> AvailableRes;
	AvailableRes.Add(FUModGameResolution(1200, 650, false));
	if (width >= 1600 && height >= 900) {
		AvailableRes.Add(FUModGameResolution(1600, 900, false));
	}
	AvailableRes.Add(FUModGameResolution(width, height, true));
	return AvailableRes;
}

void UUModGameEngine::RunPollServer(FString ip, ULocalPlayer* const Player)
{
	Player->PlayerController->ClientTravel(ip, ETravelType::TRAVEL_Absolute);
	SendPollPacket = true;
}

void UUModGameEngine::SetLoadData(int32 totalObjs, int32 curObjs, FString loadText)
{
	FLoadData data = FLoadData(loadText, totalObjs, curObjs);
	LoadDataChangeDelegate.Broadcast(data);
}