// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/Console.h"
#include "ConsoleDestroyer.generated.h"

/**
 * 
 */
UCLASS()
class UMOD_API UConsoleDestroyer : public UConsole //Console = TRASH
{
	GENERATED_BODY()
	
public:
	virtual bool ConsoleActive() const;
	virtual void ConsoleCommand(const FString& Command);
	virtual void BeginState_Open(FName PreviousStateName);
};
