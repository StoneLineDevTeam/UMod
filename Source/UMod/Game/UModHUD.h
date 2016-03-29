// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "UModHUD.generated.h"

class AUModCharacter;
class UUModGameInstance;

UCLASS()
class AUModHUD : public AHUD
{
	GENERATED_BODY()

public:
	AUModHUD(const FObjectInitializer& ObjectInitializer);

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	virtual void BeginPlay();

	FVector2D ScreenSize();

	//virtual void BeginDestroy();

private:
	/** Textures */
	uint32 CrosshairTex;

	uint32 HUDFont;

	void MainHUDRender();
	void DrawIngameMenu();
	void DrawPlayerStats(AUModCharacter *localPlyCL);
	void DrawWeaponStats(AUModCharacter *localPlyCL);
	void DrawWeaponSwitch(AUModCharacter *localPlyCL);
	void DrawUnderwaterHUD();

	void OnButtonClick(uint8 id);

	UUModGameInstance *Game;
	
};

