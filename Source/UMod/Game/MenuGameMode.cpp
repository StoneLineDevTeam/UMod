// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "MenuGameMode.h"
#include "Player/UModPlayerState.h"
#include "Player/UModController.h"

AMenuGameMode::AMenuGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// set default pawn class to our Blueprinted character
	DefaultPlayerName = NSLOCTEXT("UMod", "PlayerName", "MenuPlayer");
	PlayerControllerClass = AUModController::StaticClass();
	PlayerStateClass = AUModPlayerState::StaticClass();
}

