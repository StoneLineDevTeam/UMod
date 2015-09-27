// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UMod.h"
#include "UModHUD.h"
#include "Engine/Canvas.h"
#include "TextureResource.h"
#include "CanvasItem.h"

#include "Player/UModCharacter.h"

#include "UModGameInstance.h"

const uint8 TEXT_ALIGN_CENTER = 1;
const uint8 TEXT_ALIGN_LEFT = 0;
const uint8 TEXT_ALIGN_RIGHT = 2;

const FColor Back = FColor(50, 50, 50, 255);
const FColor DarkGrey = FColor(0, 0, 0, 255);
const FColor Black = FColor(0, 0, 0, 255);
const FColor White = FColor(255, 255, 255, 255);
const FColor Red = FColor(55, 0, 0, 255);
const FColor Green = FColor(0, 255, 0, 255);
const FColor Yellow = FColor(255, 255, 0, 255);
const FColor AmmoCol = FColor(255, 180, 0, 255);

//((void (*)(void)) ptr)();

struct Button {	
	FString text;
	uint8 id;

	Button(uint8 i, FString t) {		
		text = t;
		id = i;
	}
};

const Button* Buttons[16] = {
	new Button(0, FString("Back to game")),
	new Button(1, FString("Disconnect"))
};

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
	FCanvasTileItem Rectangle(FVector2D(X, Y), FVector2D(W, H), Color);
	Rectangle.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(Rectangle);
}

//1 = Alligment left
//2 = Alligment center
//3 = Alligment right
void AUModHUD::DrawSimpleText(FString String, float X, float Y, float Size, FColor Color, uint8 Alligment)
{
	float TW, TH;
	GetTextSize(String, TW, TH, HUDFont, Size);

	float TextX = 0;
	float TextY = Y;

	switch (Alligment)
	{
	case 0:
		TextX = X;		
		break;
	case 1:
		TextX = X - TW / 2;		
		break;
	case 2:
		TextX = X - TW;		
		break;
	}

	FCanvasTextItem Text(FVector2D(TextX, TextY), FText::FromString(String), HUDFont, Color);
	Text.Scale.Set(Size, Size);
	Canvas->DrawItem(Text);
}

FVector2D AUModHUD::ScreenSize()
{
	FVector2D ScrSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	return ScrSize;
}

bool IsMouseInRect(float msPosX, float msPosY, float x, float y, float w, float h)
{
	if (msPosX >= x && msPosX <= (x + w)) {
		if (msPosY >= y && msPosY <= (y + h)) {
			return true;
		}
	}
	return false;
}

bool IngameMenu = false;
bool clickDown = false;
bool escapeDown = false;
void AUModHUD::OnButtonClick(uint8 id)
{
	APlayerController *ctrl = GetOwningPlayerController();
	switch (id)	{
	case 0:
		IngameMenu = !IngameMenu;
		ctrl->bShowMouseCursor = IngameMenu;
		ctrl->SetIgnoreLookInput(IngameMenu);
		ctrl->SetIgnoreMoveInput(IngameMenu);
		escapeDown = false;
		break;
	case 1:		
		UUModGameInstance *game = Cast<UUModGameInstance>(GetGameInstance());
		game->Disconnect(FString("You disconnected !"));
	}
}
void AUModHUD::DrawIngameMenu()
{
	APlayerController *ctrl = GetOwningPlayerController();

	FVector2D size = ScreenSize();

	float msPosX;
	float msPosY;

	ctrl->GetMousePosition(msPosX, msPosY);
	
	DrawSimpleText("UMod - IngameMenu", size.X / 2, 100, 4, DarkGrey, TEXT_ALIGN_CENTER);
	for (int i = 0; i < 16; i++) {
		const Button* but = Buttons[i];
		if (but != NULL) {
			if (IsMouseInRect(msPosX, msPosY, size.X / 2 - 256, 200 + i * 70, 512, 64)) {
				if (ctrl->IsInputKeyDown(EKeys::LeftMouseButton)) {
					clickDown = true;
				} else if (!ctrl->IsInputKeyDown(EKeys::LeftMouseButton) && clickDown) {
					clickDown = false;
					OnButtonClick(but->id);
				}
				DrawRect(size.X / 2 - 256, 200 + i * 70, 512, 64, FColor(200, 200, 200, 200));
			} else {
				DrawRect(size.X / 2 - 256, 200 + i * 70, 512, 64, FColor(133, 133, 133, 200));				
			}			
			DrawSimpleText(but->text, size.X / 2 - 256, 200 + i * 70 + 10, 4, White, TEXT_ALIGN_LEFT);
		}
	}
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
	APlayerController *ctrl = GetOwningPlayerController();

	//DrawText(FString("Test"), FVector2D(0, 0), HUDFont, FVector2D()
	if (localPlyCL == NULL || localPlyCL->PlayerState == NULL || ctrl == NULL) {
		DrawSimpleText(FString("THIS CLIENT IS ERRORED"), 16, 16, 2, FColor(255, 0, 0),1);
		return;
	}
	FString txt = localPlyCL->PlayerState->PlayerName;	
	DrawSimpleText(FString("Playing as ") + txt, 16, 16, 2, FColor(255, 0, 0),1);
	
	//Health and Armor Box

	//Variables	
	uint32 Health = localPlyCL->GetHealth(); //Yeah I know, I could add that more quickly... @See UModCharacter.h for usefull functions : Now blueprint compatible ! Use "UMod Specific" category on blueprints to manage the player health or use C++.
	uint32 MaxHealth = localPlyCL->GetMaxHealth(); //Yeah I know, I could add that more quickly... @See UModCharacter.h for usefull functions : Now blueprint compatible ! Use "UMod Specific" category on blueprints to manage the player health or use C++.
	int Armor = 45; //Why !? There is no armor feature planned, if you want armor feature tell me.
	int Ammo = 78; //TODO : Finish weapon base
	int MaxAmmo = 100; //TODO : Finish weapon base
	int ReloadAmmo = 300; //TODO : Finish weapon base

	const FString WeaponName = "MyWeapon"; //Well about weapons, I'm realy sorry but I'm still not done...
	DrawRect(5, (size.Y / 7 * 6) - 5, size.X / 4.5, size.Y / 7, Back);
	float BoxSizeX = size.X / 4.5;
	float BoxSizeY = size.Y / 7;
	DrawSimpleText(FString("Health : ") + FString::FromInt(Health), 10, (size.Y / 7 * 6) + 5, 1.5, White, TEXT_ALIGN_LEFT);
	DrawRect(10, ((size.Y / 7 * 6) - 5) + (size.Y / 6 / 3) - 4 * 2, BoxSizeX-10, size.Y / 7 / 5.8, DarkGrey);
	DrawSimpleText(FString("Armor : ") + FString::FromInt(Armor), 10, (size.Y / 7 * 6) + (BoxSizeY / 2), 1.5, White, TEXT_ALIGN_LEFT);
	DrawRect(10, ((size.Y / 7 * 6) - 5) + (BoxSizeY / 4 * 3), BoxSizeX - 10, (size.Y / 7 / 5.8), DarkGrey);

	if (Health >= MaxHealth / 2 && Health <= 100) {
		DrawRect(10, ((size.Y / 7 * 6) - 5) + (size.Y / 6 / 3) - 4 * 2, (BoxSizeX - 10) / MaxHealth * Health, size.Y / 7 / 5.8, Green);
	} else if (Health > 20 && Health < 50) {
		DrawRect(10, ((size.Y / 7 * 6) - 5) + (size.Y / 6 / 3) - 4 * 2, (BoxSizeX - 10) / MaxHealth * Health, size.Y / 7 / 5.8, Yellow);
	} else if (Health <= 20) {
		DrawRect(10, ((size.Y / 7 * 6) - 5) + (size.Y / 6 / 3) - 4 * 2, (BoxSizeX - 10) / MaxHealth * Health, size.Y / 7 / 5.8, Red);
	}

	if (Armor > 100) { //Not planned, if you realy want armor management, then tell me.
		DrawRect(10, ((size.Y / 7 * 6) - 5) + (BoxSizeY / 4 * 3), BoxSizeX - 10, (size.Y / 7 / 5.8), Green);
	} else if (Armor >= 50 && Armor <= 100) {
		DrawRect(10, ((size.Y / 7 * 6) - 5) + (BoxSizeY / 4 * 3), (BoxSizeX - 10) / 100 * Armor, (size.Y / 7 / 5.8), Green);
	} else if (Armor > 20 && Armor < 50) {
		DrawRect(10, ((size.Y / 7 * 6) - 5) + (BoxSizeY / 4 * 3), (BoxSizeX - 10) / 100 * Armor, (size.Y / 7 / 5.8), Yellow);
	} else if (Armor <= 20) {
		DrawRect(10, ((size.Y / 7 * 6) - 5) + (BoxSizeY / 4 * 3), (BoxSizeX - 10) / 100 * Armor, (size.Y / 7 / 5.8), Red);
	}
	
	//Ammo and Weapon HUD
	float AmmoSizeX = size.X / 4.5;
	float AmmoSizeY = size.Y / 9;
	float AmmoPerc = (Ammo*100 / MaxAmmo* 100) / 100;
	DrawRect((size.X /4.5 *3.5) - 5, (size.Y / 9 * 8) - 5, size.X / 4.5, size.Y / 9, Back);
	DrawSimpleText(WeaponName, size.X / 4.5 *3.5, (size.Y / 9 * 8), 1.5, White, TEXT_ALIGN_LEFT);
	DrawRect((size.X / 4.5 *3.5), (size.Y / 9 * 8) - 5 + AmmoSizeY/2, AmmoSizeX - 10, AmmoSizeY / 3, DarkGrey);
	DrawRect((size.X / 4.5 *3.5), (size.Y / 9 * 8) - 5 + AmmoSizeY / 2, ((AmmoSizeX - 10) / 100) * AmmoPerc, AmmoSizeY / 3, AmmoCol);
	DrawSimpleText(FString::FromInt(Ammo) + FString(" | ") + FString::FromInt(ReloadAmmo), size.X / 4.5 *3.5, (size.Y / 9 * 8) + AmmoSizeY / 2, 1.5, White, TEXT_ALIGN_LEFT);

	//Weapon switch
	for (int i = 0; i < 16; i++) {
		AWeaponBase *b = localPlyCL->GetWeapons()[i];
		if (b != NULL) {
			DrawRect(size.X / 2 - (64 * 8) + i * 64, 10, 64, 64, FColor(0, 0, 0, 200));
			DrawSimpleText(b->GetNiceName(), size.X / 2 - (64 * 8) + i * 64, 10, 1, White, TEXT_ALIGN_LEFT);
		}
	}


	//This code must rest at the end of this function !	
	if (ctrl->IsInputKeyDown(EKeys::Escape)) {
		escapeDown = true;
	} else if (escapeDown && !ctrl->IsInputKeyDown(EKeys::Escape)) {
		IngameMenu = !IngameMenu;
		ctrl->bShowMouseCursor = IngameMenu;
		ctrl->SetIgnoreLookInput(IngameMenu);
		ctrl->SetIgnoreMoveInput(IngameMenu);
		escapeDown = false;
	}
	if (IngameMenu) {
		DrawIngameMenu();
	}
}

/*void AUModHUD::BeginDestroy()
{
	for (int i = 0; i < 16; i++) {
		const Button *but = Buttons[i];
		if (but != NULL) {
			delete but;
		}
	}
	delete [] &Buttons;

	delete CrosshairTex;
	delete HUDFont;
}*/