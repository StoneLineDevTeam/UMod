// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "UModConsoleManager.h"


void UUModConsoleManager::BeginDestroy()
{
	GLog->RemoveOutputDevice(this);
	OnLogAdded.RemoveAll(this);
	Super::BeginDestroy();
}

TArray<FLogLine>& UUModConsoleManager::GetLogs()
{
	return Logs;
}

void UUModConsoleManager::AddLogLine(FString log, FColor col)
{
	if (Logs.Num() == 2048) {
		Logs.RemoveAt(0);
	}
	FLogLine line = FLogLine(col, log);
	Logs.Add(line);
	OnLogAdded.Broadcast(line);
}

void UUModConsoleManager::Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const class FName& Category)
{
	FColor col;
	switch (Verbosity) {
	case ELogVerbosity::Type::Error:
		col = FColorList::Red;
		break;
	case ELogVerbosity::Type::Warning:
		col = FColorList::Yellow;
		break;
	case ELogVerbosity::Type::Log:
		col = FColorList::Black;
		break;
	}
	FString str = FString("[");
	str += Category.ToString();
	str += "]";
	str += " -> ";
	str += FString(V);
	AddLogLine(str, col);
}

