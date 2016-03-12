// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "UModGameMode.generated.h"

class AUModCharacter;
class UUModGameInstance;

UCLASS(minimalapi)
class AUModGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AUModGameMode(const FObjectInitializer& ObjectInitializer);

	void OnPlayerDeath(AUModCharacter *player);

	void OnPlayerInitialSpawn(AUModCharacter *player);

	void OnPlayerSpawn(AUModCharacter *player);

	virtual APlayerController *Login(class UPlayer *NewPlayer, ENetRole InRemoteRole, const FString & Portal, const FString & Options, const TSharedPtr< const FUniqueNetId > & UniqueId, FString & ErrorMessage);

	virtual void PostLogin(APlayerController *newCtrl);

	virtual void Logout(AController *ctrl);

private:
	FString LuaGameMode;
	UUModGameInstance *Game;
};