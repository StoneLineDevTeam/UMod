// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "UModGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UMOD_API UUModGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	
public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Log Message", Keywords = "ue_log log"), Category = UMod_Specific)
		static void LogMessage(FString msg, uint8 level);

	virtual void Init();
	virtual void Shutdown();
};
