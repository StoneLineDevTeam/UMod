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
	void InitPlayerConnection(uint32 maxTicks);

	virtual void Tick(float f);

private:
	uint32 MaxElapsedTicks;
	uint32 CurrentElapsedTicks;
	bool RunTryConnect;

	UFUNCTION(Server, Reliable, WithValidation)
	void SendDefineRequest();
	void SendDefineRequest_Implementation();
	bool SendDefineRequest_Validate();

	UFUNCTION(Client, Reliable)
	void RespondToDefineRequest();
	void RespondToDefineRequest_Implementation();
};
