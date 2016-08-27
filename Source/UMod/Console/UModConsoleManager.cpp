// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "UModConsoleManager.h"

#include "UModGameInstance.h"
#include "UModGameEngine.h"
#include "Player/UModController.h"

FUModConsoleCommand** UUModConsoleManager::ConsoleCommands = new FUModConsoleCommand*[128];
int UUModConsoleManager::ConsoleCommandNumber = 0;

void UUModConsoleManager::BeginDestroy()
{	
	GLog->RemoveOutputDevice(this);
	OnLogAdded.RemoveAll(this);
	Super::BeginDestroy();
}

TArray<FLogLine>& UUModConsoleManager::GetLogs()
{
	return Logs;
}

void UUModConsoleManager::AddLogLine(FString log, FColor col)
{
	if (Logs.Num() == 2048) {
		Logs.RemoveAt(0);
	}
	FLogLine line = FLogLine(col, log);
	Logs.Add(line);
	OnLogAdded.Broadcast(line);
}

void UUModConsoleManager::Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category)
{
	FColor col;
	switch (Verbosity) {
	case ELogVerbosity::Type::Error:
		col = FColorList::Red;
		break;
	case ELogVerbosity::Type::Warning:
		col = FColorList::Yellow;
		break;
	case ELogVerbosity::Type::Log:
		col = FColorList::Black;
		break;
	}
	FString str = FString("[");
	str += Category.ToString();
	str += "]";
	str += " -> ";
	str += FString(V);
	AddLogLine(str, col);
}

void UUModConsoleManager::DefineConsoleInt(FUModConsoleVar<int> var)
{
	ConsoleIntegers.Add(var);
}

void UUModConsoleManager::DefineConsoleString(FUModConsoleVar<FString> var)
{
	ConsoleStrings.Add(var);
}

void UUModConsoleManager::DefineConsoleBool(FUModConsoleVar<bool> var)
{
	ConsoleBooleans.Add(var);
}

void UUModConsoleManager::RunConsoleCommand(FString cmdline, AUModCharacter* player)
{
	TArray<FString> args;
	cmdline.ParseIntoArray(args, TEXT(" "), false);
	if (args.Num() == 0) {
		UE_LOG(UMod_Input, Error, TEXT("Null command entered, parsing aborted."));
		return;
	}
	FString cmd = args[0];
	args.RemoveAt(0);
	for (int i = 0; i < ConsoleCommandNumber; i++) {
		if (ConsoleCommands[i]->CommandName == cmd) {
			if (player == NULL && ConsoleCommands[i]->NeedPlayer) {
				UE_LOG(UMod_Input, Error, TEXT("This command requires a player context."));
				return;
			}
			
			if (!ConsoleCommands[i]->ExecFunc(player, Game, args)) {
				UE_LOG(UMod_Input, Error, TEXT("An error has occured while performing command."));
				return;
			} else {
				return;
			}
		}
	}
	UE_LOG(UMod_Input, Error, TEXT("The command '%s' does not exist."), *cmd);
}

bool ReloadCMD(AUModCharacter* player, UUModGameInstance* Game, TArray<FString> args)
{
	Game->ReloadLua();
	UE_LOG(UMod_Input, Log, TEXT("Lua VM has been reloaded."));
	return true;
}
bool ChangeMapCMD(AUModCharacter* player, UUModGameInstance* Game, TArray<FString> args)
{
	if (args.Num() == 0) {
		UE_LOG(UMod_Input, Error, TEXT("Not enough arguments provided."));
		return false;
	}
	FString newMap = args[0];
	FString realPath;
	EResolverResult res = Game->AssetsManager->ResolveAsset(newMap, EUModAssetType::MAP, realPath);
	if (res != EResolverResult::SUCCESS) {
		UE_LOG(UMod_Input, Error, TEXT("Asset resolver returned : %s"), *Game->AssetsManager->GetErrorMessage(res));
		return false;
	}
	//Broadcast info to all clients to reconnect
	for (TObjectIterator<AUModController> Itr; Itr; ++Itr) {
		if (*Itr != NULL && Itr->GetNetConnection() != NULL) {
			FNetControlMessage<NMT_UModChangeMap>::Send(Itr->GetNetConnection());
		}
	}
	//End
	UGameplayStatics::OpenLevel(Game->GetWorld(), FName(*realPath), false);
	UE_LOG(UMod_Input, Log, TEXT("Changing map."));
	return true;
}
bool SlowMotionCMD(AUModCharacter* player, UUModGameInstance* Game, TArray<FString> args)
{
	if (args.Num() == 0) {
		UE_LOG(UMod_Input, Error, TEXT("Not enough arguments provided."));
		return false;
	}
	if (args[0] == "ON") {
		Game->GetWorld()->GetWorldSettings()->TimeDilation = 0.25F;
		UE_LOG(UMod_Input, Log, TEXT("Slow motion enabled."));
	} else if (args[0] == "OFF") {
		Game->GetWorld()->GetWorldSettings()->TimeDilation = 1;
		UE_LOG(UMod_Input, Log, TEXT("Slow motion disabled."));
	}
	return true;
}
bool ExitCMD(AUModCharacter* player, UUModGameInstance* Game, TArray<FString> args) {
	FPlatformMisc::RequestExit(false);
	return true;
}
DECLARE_UMOD_COMMAND(0, "RELOAD", "Reloads lua VM", false, ReloadCMD)
DECLARE_UMOD_COMMAND(1, "CHMAP", "Changes current game map", false, ChangeMapCMD)
DECLARE_UMOD_COMMAND(2, "SLOWMO", "Toggles slow motion (args : ON/OFF)", false, SlowMotionCMD)
DECLARE_UMOD_COMMAND(3, "EXIT", "Shuts down the game server", false, ExitCMD)
