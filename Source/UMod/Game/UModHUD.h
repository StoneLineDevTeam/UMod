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

	UFUNCTION()
	void Precache();

	UFUNCTION()
	void HandleMouseClick(float x, float y);

private:
	/** Textures */
	uint32 CrosshairTex;

	uint32 HUDFont;

	void MainHUDRender();
	void DrawIngameMenu();
	void DrawPlayerStats();
	void DrawWeaponStats();
	void DrawWeaponSwitch();
	void DrawUnderwaterHUD();

	void OnButtonClick(uint8 id);

	UUModGameInstance *Game;
	AUModController *PlyCtrl;
};

