// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "ConsoleDestroyer.h"


bool UConsoleDestroyer::ConsoleActive() const
{
	return false;
}

void UConsoleDestroyer::ConsoleCommand(const FString& Command)
{
	
}

void UConsoleDestroyer::BeginState_Open(FName PreviousStateName)
{
	Super::FakeGotoState(NAME_None);
	
}
