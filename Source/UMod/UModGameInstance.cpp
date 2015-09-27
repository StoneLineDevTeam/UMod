// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "UModGameInstance.h"

#include "Game/UModGameMode.h"

const FString GVersion = FString("0.1 - Alpha");
const FString LuaVersion = FString("NULL");
const FString LuaEngineVersion = FString("NULL");

UUModGameInstance::UUModGameInstance(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UUModGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UUModGameInstance::OnStartOnlineGameComplete);
}

void UUModGameInstance::LogMessage(FString msg, uint8 level)
{
	switch (level)
	{
	case 0:
		UE_LOG(UMod_Game, Warning, TEXT("%s"), *msg);
		break;
	case 1:
		UE_LOG(UMod_Game, Error, TEXT("%s"), *msg);
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

void OnNetworkFailure(UWorld *world, UNetDriver *driver, ENetworkFailure::Type failType, const FString &ErrorMessage)
{

}

//Game startup
void UUModGameInstance::Init()
{
	int32 vers = GEngine->GetLinkerUE4Version();
	
	FString lua = FString("LuaEngine V.") + LuaEngineVersion + FString(" | Lua V.") + LuaVersion;

	UE_LOG(UMod_Game, Log, TEXT("UMod - V.%s | Engine V.%s"), *GetGameVersion(), *FString::FromInt(vers));
	UE_LOG(UMod_Lua, Log, TEXT("%s"), *lua);

	//GEngine->NetworkFailureEvent.AddStatic(OnNetworkFailure);
	//Found a way to set port !
	GConfig->SetInt(TEXT("URL"), TEXT("Port"), 25565, GEngineIni);
}

//Game shutdown
void UUModGameInstance::Shutdown()
{

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

	Player->PlayerController->ClientTravel("LoadScreen", ETravelType::TRAVEL_Absolute);
	
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

	CurSessionMapName = map;

	FName name = FName(*hostName);

	CurSessionName = hostName;

	return Sessions->CreateSession(*UserId, name, *SessionSettings);
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
	FString str = ip + FString(":");	
	str.AppendInt(port);
	ConnectIP = str;

	Player->PlayerController->ClientTravel("LoadScreen", ETravelType::TRAVEL_Absolute);

	DelayedServerConnect = true;

	return false;
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
	}	
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
			ticks = 0;
		}
	}

	if (!CurSessionName.IsEmpty()) {
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
	}
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

	netError = error;

	ULocalPlayer* const Player = GetFirstGamePlayer();
	Player->PlayerController->ClientTravel("LoadScreen", ETravelType::TRAVEL_Absolute);
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
	Player->PlayerController->ClientTravel("MainMenu", ETravelType::TRAVEL_Absolute);
}