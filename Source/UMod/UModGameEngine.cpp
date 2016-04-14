#include "UMod.h"
#include "UModGameEngine.h"
#include "Engine/PendingNetGame.h"
#include "Console/UModConsoleManager.h"
#include "NetCore/ServerHandler.h"
#include "NetCore/ClientHandler.h"

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

const uint32 MinResX = 1200;
const uint32 MinResY = 650;

bool UUModGameEngine::IsDedicated = false;
bool UUModGameEngine::IsListen = false;
bool UUModGameEngine::IsPollingServer = false;

bool SendPollPacket;

EBrowseReturnVal::Type UUModGameEngine::Browse(FWorldContext& WorldContext, FURL URL, FString& Error)
{
	if (URL.IsInternal() && GIsClient && !URL.IsLocalInternal()) {
		//Init network
		EBrowseReturnVal::Type t = Super::Browse(WorldContext, URL, Error);
		//Hack network now (yeah fuck you UE4)
		if (t == EBrowseReturnVal::Type::Success || t == EBrowseReturnVal::Type::Pending) {
			UNetDriver *NetDriver = GEngine->FindNamedNetDriver(WorldContext.PendingNetGame, NAME_PendingNetDriver);
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
		UE_LOG(UMod_Game, Error, TEXT("Disconnected UWorld from network notify system !"));
		return t;
	}
	//Ok relaying on the normal browse system as we don't want to change UE4's map loading system
	return Super::Browse(WorldContext, URL, Error);
}

void UUModGameEngine::NetworkCleanUp()
{
	if (GetGameWorld()->GetNetDriver() != NULL) {
		GetGameWorld()->GetNetDriver()->Shutdown();
	}
	if (NetHandleCL != NULL) {
		delete NetHandleCL;
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
	//YOUHOU ! It's overWRITE now not override !
#if UE_BUILD_SERVER
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

	FString str = "/Game/Maps/" + MapToLoad + " -log";
	FCommandLine::Set(*str);

	IsDedicated = true;
	IsListen = false;
#else
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
	} else {
		const UGameMapsSettings* GameMapsSettings = GetDefault<UGameMapsSettings>();
		GameMapsSettings->SetGameDefaultMap("/Game/Internal/Maps/MainMenu");

		IsDedicated = false;
	}
#endif

	if (!IsDedicated) {
		//Obviously dedicated server has no SWindow... Neither any viwports...
		OnDisplayCreated();
		UE_LOG(UMod_Game, Log, TEXT("Display data has been overwritten."));
		Super::Init(InEngineLoop);
		OnDisplayCreated();
		UE_LOG(UMod_Game, Log, TEXT("Display data has been re-overwritten."));
	} else {
		UE_LOG(UMod_Game, Log, TEXT("Running UMod as server application..."));
		Super::Init(InEngineLoop);
	}
}

void UUModGameEngine::OnDisplayCreated()
{
	int32 width = MinResX;
	int32 height = MinResY;
	bool full;
	GConfig->GetInt(TEXT("Viewport"), TEXT("Width"), width, FPaths::GameConfigDir() + FString("UMod.Client.cfg"));
	GConfig->GetInt(TEXT("Viewport"), TEXT("Height"), height, FPaths::GameConfigDir() + FString("UMod.Client.cfg"));
	GConfig->GetBool(TEXT("Viewport"), TEXT("FullScreen"), full, FPaths::GameConfigDir() + FString("UMod.Client.cfg"));
	GSystemResolution.ResX = width;
	GSystemResolution.ResY = height;
	if (full) {
		GSystemResolution.WindowMode = EWindowMode::Fullscreen;
	} else {
		GSystemResolution.WindowMode = EWindowMode::Windowed;
	}
	TSharedPtr<SWindow> WindowPtr = GameViewportWindow.Pin();
	WindowPtr->ReshapeWindow(FVector2D(0, 0), FVector2D(GSystemResolution.ResX, GSystemResolution.ResY));
	WindowPtr->SetWindowMode(GSystemResolution.WindowMode);
	WindowPtr->SetTitle(FText::FromString("UMod - GMod Replacement Project [Client]"));
}

bool UUModGameEngine::ChangeGameResolution(FUModGameResolution res)
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

FUModGameResolution UUModGameEngine::GetGameResolution()
{
	const UGameViewportClient* Viewport = GetWorld()->GetGameViewport();
	if (Viewport == NULL) {
		return FUModGameResolution();
	}
	FVector2D vec;
	Viewport->GetViewportSize(vec);
	return FUModGameResolution(vec.X, vec.Y, Viewport->IsFullScreenViewport());
}

TArray<FUModGameResolution> UUModGameEngine::GetAvailableGameResolutions()
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
	}
	else {
		UE_LOG(UMod_Game, Error, TEXT("Screen Resolutions could not be obtained"));
		return TArray<FUModGameResolution>();
	}
}

void UUModGameEngine::RunPollServer(FString ip, ULocalPlayer* const Player)
{
	Player->PlayerController->ClientTravel(ip, ETravelType::TRAVEL_Absolute);
	SendPollPacket = true;
}