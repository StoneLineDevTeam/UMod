// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "UModGameInstance.h"

void UUModGameInstance::LogMessage(FString msg, uint8 level)
{
	switch (level)
	{
	case 0:
		UE_LOG(UMod_Game, Warning, TEXT("%s"), *msg);
		break;
	case 1:
		UE_LOG(UMod_Game, Error, TEXT("%s"), *msg);
		break;
	}
}

void OnNetworkFailure(UWorld *world, UNetDriver *driver, ENetworkFailure::Type failType, const FString &ErrorMessage)
{

}

//Game startup
void UUModGameInstance::Init()
{
	GEngine->NetworkFailureEvent.AddStatic(OnNetworkFailure);
}

//Game shutdown
void UUModGameInstance::Shutdown()
{

}