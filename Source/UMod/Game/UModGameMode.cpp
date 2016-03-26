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
}

void AUModGameMode::BeginPlay()
{
	//Lua change : start implementing call to GM functions

	//Init Lua GameMode (only loads init.lua gamemode file currently)
	//Run test V2.0 using BeginPlay instead of constructor
	Game = Cast<UUModGameInstance>(GetGameInstance());
	FString LuaGameMode = Game->GetGameMode();
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
	} else { //This part will not work because GameMode is server only.... I need the GameState for that...
		/*if (FPaths::FileExists(FPaths::GameDir() + "/GameModes/" + LuaGameMode + "/cl_init.lua")) {
			Game->Lua->RunScript(FPaths::GameDir() + "/GameModes/" + LuaGameMode + "/cl_init.lua");
		} else {
			UE_LOG(UMod_Lua, Warning, TEXT("Could not load %s : file does not exist. Lua GM will not run."), *FString("GameModes/" + LuaGameMode + "/cl_init.lua"));
		}*/
	}
}

void AUModGameMode::OnPlayerDeath(AUModCharacter* player)
{
	//TODO : make something when player die
	player->StripWeapons();
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