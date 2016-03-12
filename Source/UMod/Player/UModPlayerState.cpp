// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "UModPlayerState.h"
#include "UModGameInstance.h"


//Client sends first that
void AUModPlayerState::SendDefineRequest_Implementation()
{
	RespondToDefineRequest();
}
bool AUModPlayerState::SendDefineRequest_Validate()
{
	return true;
}

//Server should send that
void AUModPlayerState::RespondToDefineRequest_Implementation()
{
	RunTryConnect = false;
	MaxElapsedTicks = 0;
	CurrentElapsedTicks = 0;
}

void AUModPlayerState::Tick(float f)
{
	if (Role != ROLE_Authority && RunTryConnect) {
		CurrentElapsedTicks++;
		if (CurrentElapsedTicks >= MaxElapsedTicks) {
			RunTryConnect = false;
			MaxElapsedTicks = 0;
			CurrentElapsedTicks = 0;
			UUModGameInstance *game = Cast<UUModGameInstance>(GetWorld()->GetGameInstance());
			game->Disconnect(FString("ERROR_MAX_RESPOND_DELAY_EXCEEDED"));			
		}
	}
}

void AUModPlayerState::InitPlayerConnection(uint32 maxTicks)
{
	if (Role == ROLE_Authority) {
		return;
	}
	MaxElapsedTicks = maxTicks;
	RunTryConnect = true;

	SendDefineRequest();
}