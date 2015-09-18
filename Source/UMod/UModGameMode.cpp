// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UMod.h"
#include "UModGameMode.h"
#include "UModHUD.h"
#include "UModCharacter.h"
#include "UModGameState.h"

AUModGameMode::AUModGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Internal/Blueprints/UModPlayer"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	DefaultPlayerName = NSLOCTEXT("UMod", "PlayerName", "UModPlayer");

	GameStateClass = AUModGameState::StaticClass();
	
	// use our custom HUD class
	HUDClass = AUModHUD::StaticClass();
}

void AUModGameMode::OnPlayerDeath(AUModCharacter* player)
{
	//TODO : make something when player die
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