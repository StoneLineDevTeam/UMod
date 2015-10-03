// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "MenuGameMode.h"
#include "Player/UModPlayerState.h"

AMenuGameMode::AMenuGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// set default pawn class to our Blueprinted character
	DefaultPlayerName = NSLOCTEXT("UMod", "PlayerName", "MenuPlayer");
		
	PlayerStateClass = AUModPlayerState::StaticClass();
}

