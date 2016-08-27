// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "MenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class AMenuGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AMenuGameMode(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay();
};
