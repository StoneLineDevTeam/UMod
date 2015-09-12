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

void AUModHUD::DrawRect(float X, float Y, float W, float H, FLinearColor Color)
{
	if (!Canvas) return;
	FCanvasTileItem Rectangle(
		FVector2D(X, Y),
		FVector2D(W, H),
		Color
		);
	Rectangle.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Rectangle);
}

//1 = Alligment left
//2 = Alligment center
//3 = Alligment right
void AUModHUD::DrawSimpleText(FString String, float X, float Y, float Size, FColor Color, float Alligment)
{

	if (!X) return;
	if (!Y) return;
	if (!Size) return;
	if (!Alligment) return;
	if (!Canvas) return;

	float TW, TH;
	GetTextSize(String, TW, TH, HUDFont, Size);

	float TextX = 0;
	float TextY = 0;

	if (Alligment == 1)
	{
		TextX = X;
		TextY = Y;
	}
	else if (Alligment == 2)
	{
		TextX = X - TW / 2;
		TextY = Y - TH / 2;
	}
	else if (Alligment == 3)
	{
		TextX = X - TW;
		TextY = Y - TH;
	}

	FCanvasTextItem Text(
		FVector2D(TextX, TextY),
		FText::FromString(String),
		HUDFont,
		Color
		);
	Text.Scale.Set(Size, Size);
	Canvas->DrawItem(Text);
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
		DrawSimpleText(FString("THIS CLIENT IS ERRORED"), 16, 16, 2, FColor(255, 0, 0),1);
		return;
	}
	FString txt = localPlyCL->PlayerState->PlayerName;	
	DrawSimpleText(FString("Playing as ") + txt, 16, 16, 2, FColor(255, 0, 0),1);
	
	//Health and Armor Box
	//Colors
	const FColor Back = FColor(50, 50, 50, 255);
	const FColor DarkGrey = FColor(0, 0, 0, 255);
	const FColor Black = FColor(0, 0, 0, 255);
	const FColor White = FColor(255, 255, 255, 255);
	const FColor Red = FColor(55, 0, 0, 255);
	const FColor Green = FColor(0, 255, 0, 255);
	const FColor Yellow = FColor(255, 255, 0, 255);
	const FColor AmmoCol = FColor(255, 180, 0, 255);
	//Variables
	int Health = 100;
	int Armor = 45;
	int Ammo = 78;
	int MaxAmmo = 100;
	int ReloadAmmo = 300;
	const FString WeaponName = "MyWeapon";
	AUModHUD::DrawRect(5, (size.Y / 7 * 6) - 5, size.X / 4.5, size.Y / 7, Back);
	float BoxSizeX = size.X / 4.5;
	float BoxSizeY = size.Y / 7;
	AUModHUD::DrawSimpleText(FString("Health: ") + FString::FromInt(Health), 10, (size.Y / 7 * 6) + 5, 1.5, White, 1);
	AUModHUD::DrawRect(10, ((size.Y / 7 * 6) - 5) + (size.Y / 6 / 3) - 4 * 2, BoxSizeX-10, size.Y / 7 / 5.8, DarkGrey);
	AUModHUD::DrawSimpleText(FString("Armor: ") + FString::FromInt(Armor), 10, (size.Y / 7 * 6) + (BoxSizeY / 2), 1.5, White, 1);
	AUModHUD::DrawRect(10, ((size.Y / 7 * 6) - 5) + (BoxSizeY / 4 * 3), BoxSizeX - 10, (size.Y / 7 / 5.8), DarkGrey);

	if (Health > 100)
	{
		AUModHUD::DrawRect(10, ((size.Y / 7 * 6) - 5) + (size.Y / 6 / 3) - 4 * 2, BoxSizeX - 10, size.Y / 7/ 5.8, Green);
	}
	else if (Health >= 50 && Health <= 100)
	{
		AUModHUD::DrawRect(10, ((size.Y / 7 * 6) - 5) + (size.Y / 6 / 3) - 4 * 2, (BoxSizeX - 10)/100*Health, size.Y / 7 / 5.8, Green);
	}
	else if (Health > 20 && Health < 50)
	{
		AUModHUD::DrawRect(10, ((size.Y / 7 * 6) - 5) + (size.Y / 6 / 3) - 4 * 2, (BoxSizeX - 10) / 100 * Health, size.Y / 7 / 5.8, Yellow);
	}
	else if (Health <= 20)
	{
		AUModHUD::DrawRect(10, ((size.Y / 7 * 6) - 5) + (size.Y / 6 / 3) - 4 * 2, (BoxSizeX - 10) / 100 * Health, size.Y / 7 / 5.8, Red);
	}

	if (Armor > 100)
	{
		AUModHUD::DrawRect(10, ((size.Y / 7 * 6) - 5) + (BoxSizeY / 4 * 3), BoxSizeX - 10, (size.Y / 7 / 5.8), Green);
	}
	else if (Armor >= 50 && Armor <= 100)
	{
		AUModHUD::DrawRect(10, ((size.Y / 7 * 6) - 5) + (BoxSizeY / 4 * 3), (BoxSizeX - 10) / 100 * Armor, (size.Y / 7 / 5.8), Green);
	}
	else if (Armor > 20 && Armor < 50)
	{
		AUModHUD::DrawRect(10, ((size.Y / 7 * 6) - 5) + (BoxSizeY / 4 * 3), (BoxSizeX - 10) / 100 * Armor, (size.Y / 7 / 5.8), Yellow);
	}
	else if (Armor <= 20)
	{
		AUModHUD::DrawRect(10, ((size.Y / 7 * 6) - 5) + (BoxSizeY / 4 * 3), (BoxSizeX - 10) / 100 * Armor, (size.Y / 7 / 5.8), Red);
	}
	
	//Ammo and Weapon HUD
	float AmmoSizeX = size.X / 4.5;
	float AmmoSizeY = size.Y / 9;
	float AmmoPerc = (Ammo*100 / MaxAmmo* 100) / 100;
	AUModHUD::DrawRect((size.X /4.5 *3.5) - 5, (size.Y / 9 * 8) - 5, size.X / 4.5, size.Y / 9, Back);
	AUModHUD::DrawSimpleText(WeaponName, size.X / 4.5 *3.5, (size.Y / 9 * 8) , 1.5, White, 1);
	AUModHUD::DrawRect((size.X / 4.5 *3.5), (size.Y / 9 * 8) - 5 + AmmoSizeY/2, AmmoSizeX - 10, AmmoSizeY / 3, DarkGrey);
	AUModHUD::DrawRect((size.X / 4.5 *3.5), (size.Y / 9 * 8) - 5 + AmmoSizeY / 2, ((AmmoSizeX - 10) / 100) * AmmoPerc, AmmoSizeY / 3, AmmoCol);
	AUModHUD::DrawSimpleText(FString::FromInt(Ammo) + FString("|") + FString::FromInt(ReloadAmmo), size.X / 4.5 *3.5, (size.Y / 9 * 8) + AmmoSizeY / 2, 1.5, White, 1);
	//AUModHUD::DrawSimpleText(FString::SanitizeFloat(AmmoPerc), size.X / 4.5 *3.5, (size.Y / 9 * 1) + AmmoSizeY / 2, 1.5, Red, 1);
	//AUModHUD::DrawRect((size.X / 4.5 *3.5) - 5, (size.Y / 9 * 8) - 5, size.X / 4.5, size.Y / 9, Back);
	//AGameState* const gameState = GetWorld() != NULL ? GetWorld()->GetGameState<>() : NULL;
}