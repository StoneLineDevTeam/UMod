// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "UModPlayerState.h"
#include "UModGameInstance.h"

void AUModPlayerState::Tick(float f)
{
}

void AUModPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority) {

	}
}