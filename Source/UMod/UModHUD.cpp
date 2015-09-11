// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UMod.h"
#include "UModHUD.h"
#include "Engine/Canvas.h"
#include "TextureResource.h"
#include "CanvasItem.h"

#include "UModCharacter.h"

AUModHUD::AUModHUD(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshiarTexObj(TEXT("/Game/Internal/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshiarTexObj.Object;

	static ConstructorHelpers::FObjectFinder<UFont> HUDFontObj(TEXT("/Game/Fonts/FederationRegular"));
	HUDFont = HUDFontObj.Object;
}

void AUModHUD::DrawSimpleText(FString text, float x, float y, int size, FColor color)
{
	FVector2D scr = ScreenSize();
	
	FVector2D posVec = FVector2D(x - scr.X * 0.5F, y - scr.Y * 0.5F);
	FVector2D sizeVec = FVector2D(size, size);

	DrawText(text, posVec, HUDFont, sizeVec, color);
}

FVector2D AUModHUD::ScreenSize()
{
	FVector2D ScrSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	return ScrSize;
}

void AUModHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition((Center.X - (CrosshairTex->GetSurfaceWidth() * 0.5)), (Center.Y - (CrosshairTex->GetSurfaceHeight() * 0.5f)));

	// draw the crosshair
	FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);

	FVector2D size = ScreenSize();

	AUModCharacter *localPlyCL = Cast<AUModCharacter>(GetOwningPawn());
	//DrawText(FString("Test"), FVector2D(0, 0), HUDFont, FVector2D()
	if (localPlyCL == NULL || localPlyCL->PlayerState == NULL) {
		DrawSimpleText(FString("THIS CLIENT IS ERRORED"), 16, 16, 2, FColor(255, 0, 0));
		return;
	}
	FString txt = localPlyCL->PlayerState->PlayerName;	
	DrawSimpleText(FString("Playing as ") + txt, 16, 16, 2, FColor(255, 0, 0));
	
	//AGameState* const gameState = GetWorld() != NULL ? GetWorld()->GetGameState<>() : NULL;
}