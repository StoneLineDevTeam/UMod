// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UMod.h"
#include "UModGameInstance.h"
#include "Game/UModGameMode.h"
#include "UModHUD.h"
#include "Player/UModCharacter.h"
#include "Player/UModController.h"
#include "Player/UModPlayerState.h"
#include "Game/UModGameState.h"

#include "Entities/Weapons/WeaponPhysgun.h"

AUModGameMode::AUModGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Internal/Blueprints/UModPlayer"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	DefaultPlayerName = NSLOCTEXT("UMod", "PlayerName", "UModPlayer");

	GameStateClass = AUModGameState::StaticClass();

	PlayerControllerClass = AUModController::StaticClass();
	
	HUDClass = AUModHUD::StaticClass();

	PlayerStateClass = AUModPlayerState::StaticClass();

	//Lua change : start implementing call to GM functions
	GConfig->GetString(TEXT("Common"), TEXT("GameMode"), LuaGameMode, FPaths::GameConfigDir() + FString("UMod.Server.cfg"));
	if (LuaGameMode.IsEmpty()) {
		LuaGameMode = FString("");
		GConfig->SetString(TEXT("Common"), TEXT("GameMode"), *LuaGameMode, FPaths::GameConfigDir() + FString("UMod.Server.cfg"));
	}
	
	//Init Lua GameMode (only loads init.lua gamemode file currently)
	//Tried to init lua : result = crash because of UE4 : GetGameInstance crashes AActor
	//I'll try a few hacks/tricks in case I can fix this (at least I have the GetInstance way... (Yeah I know it's not realy clean but if UE4 wants to block me I have to fuck it up !)
	//If I can't get the game instance from an Actor then it's already finished...
	/*Game = Cast<UUModGameInstance>(GetGameInstance());
	if (Game == NULL) {
		UE_LOG(UMod_Game, Error, TEXT("UModGameInstance failed to retrieve in UModGameMode. Lua may be disfunctional..."));
		return;
	}
	if (Role == ROLE_Authority) {
		if (FPaths::FileExists(FPaths::GameDir() + "/GameModes/" + LuaGameMode + "/init.lua")) {
			Game->Lua->RunScript(FPaths::GameDir() + "/GameModes/" + LuaGameMode + "/init.lua");
		} else {
			UE_LOG(UMod_Lua, Warning, TEXT("Could not load %s : file does not exist. Lua GM will not run."), *FString("GameModes/" + LuaGameMode + "/init.lua"));
		}
	} else {
		if (FPaths::FileExists(FPaths::GameDir() + "/GameModes/" + LuaGameMode + "/cl_init.lua")) {
			Game->Lua->RunScript(FPaths::GameDir() + "/GameModes/" + LuaGameMode + "/cl_init.lua");
		} else {
			UE_LOG(UMod_Lua, Warning, TEXT("Could not load %s : file does not exist. Lua GM will not run."), *FString("GameModes/" + LuaGameMode + "/cl_init.lua"));
		}
	}*/
}

void AUModGameMode::OnPlayerDeath(AUModCharacter* player)
{
	//TODO : make something when player die
}

void AUModGameMode::OnPlayerInitialSpawn(AUModCharacter *player)
{
	player->GiveWeapon(AWeaponPhysgun::StaticClass());
}

//Loggs in the player
APlayerController *AUModGameMode::Login(class UPlayer *NewPlayer, ENetRole InRemoteRole, const FString & Portal, const FString & Options, const TSharedPtr< const FUniqueNetId > & UniqueId, FString & ErrorMessage)
{
	return Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
}

//Player disconnected (Server)
void AUModGameMode::Logout(AController *ctrl)
{
	Super::Logout(ctrl);
}

//Login Success
void AUModGameMode::PostLogin(APlayerController *ctrl)
{
	Super::PostLogin(ctrl);
}