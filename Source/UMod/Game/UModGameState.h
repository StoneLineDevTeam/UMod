// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "UModGameState.generated.h"

UCLASS()
class AUModGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	virtual void HandleMatchIsWaitingToStart();

	FVector SpawnPos;
	FRotator SpawnRot;
};
