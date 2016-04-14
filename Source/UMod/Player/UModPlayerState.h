// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "UModPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class UMOD_API AUModPlayerState : public APlayerState
{
	GENERATED_BODY()
		
public:
	virtual void Tick(float f);
	virtual void BeginPlay();

private:
	
};
