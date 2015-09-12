// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "UModGameMode.generated.h"

class AUModCharacter;

UCLASS(minimalapi)
class AUModGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AUModGameMode(const FObjectInitializer& ObjectInitializer);

	void OnPlayerDeath(AUModCharacter* player);
};



