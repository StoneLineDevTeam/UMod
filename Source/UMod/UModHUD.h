// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "UModHUD.generated.h"

UCLASS()
class AUModHUD : public AHUD
{
	GENERATED_BODY()

public:
	AUModHUD(const FObjectInitializer& ObjectInitializer);

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	void DrawSimpleText(FString String, float X, float Y, float Size, FColor Color, uint8 Alligment);
	void DrawRect(float X, float Y, float W, float H, FLinearColor Color);

	FVector2D ScreenSize();

	//virtual void BeginDestroy();

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

	class UFont* HUDFont;

	void DrawIngameMenu();

	void OnButtonClick(uint8 id);
	
};

