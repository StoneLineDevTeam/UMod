// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UMod.h"
#include "UModHUD.h"
#include "Engine/Canvas.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "Renderer/Render2D.h"

#include "Player/UModCharacter.h"
#include "Player/UModController.h"

#include "UModGameInstance.h"

const FColor Back = FColor(50, 50, 50, 255);
const FColor DarkGrey = FColor(0, 0, 0, 255);
const FColor Black = FColor(0, 0, 0, 255);
const FColor White = FColor(255, 255, 255, 255);
const FColor Red = FColor(55, 0, 0, 255);
const FColor Green = FColor(0, 255, 0, 255);
const FColor Yellow = FColor(255, 255, 0, 255);
const FColor AmmoCol = FColor(255, 180, 0, 255);

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
	UUModAssetsManager::PrecacheAssets.AddDynamic(this, &AUModHUD::Precache);
}

void AUModHUD::Precache()
{
	CrosshairTex = URender2D::LoadTexture("Internal:Textures/FirstPersonCrosshair");
	HUDFont = URender2D::LoadFont("UMod:FederationRegular", 32);
}

void AUModHUD::BeginPlay()
{
	Super::BeginPlay();

	Game = Cast<UUModGameInstance>(GetGameInstance());
	PlyCtrl = Cast<AUModController>(GetOwningPlayerController());

	PlyCtrl->OnMouseClick.AddDynamic(this, &AUModHUD::HandleMouseClick);

	UE_LOG(UMod_Game, Log, TEXT("[DEBUG]BeginPlay called for HUD"));
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

void AUModHUD::OnButtonClick(uint8 id)
{	
	switch (id)	{
	case 0:		
		PlyCtrl->ExitIngameMenu();
		break;
	case 1:
		Game->Disconnect(FString("You disconnected !"));
	}
}

void AUModHUD::HandleMouseClick(float x, float y)
{
	FVector2D size = ScreenSize();

	for (int i = 0; i < 16; i++) {
		const Button* but = Buttons[i];
		if (but != NULL) {
			if (FVUIUtils::IsMouseInRect(size.X / 2 - 256, 200 + i * 70, 512, 64)) {
				OnButtonClick(but->id);
			}
		}
	}
}

void AUModHUD::DrawIngameMenu()
{
	FVector2D size = ScreenSize();

	float msPosX;
	float msPosY;

	PlyCtrl->GetMousePosition(msPosX, msPosY);
	
	URender2D::SetColor(DarkGrey);
	URender2D::SetFontScale(1, 1);
	URender2D::DrawText("UMod - " + Game->GetGameMode(), size.X / 2, 100, TEXT_ALIGN_CENTER);
	for (int i = 0; i < 16; i++) {
		const Button* but = Buttons[i];
		if (but != NULL) {
			if (FVUIUtils::IsMouseInRect(size.X / 2 - 256, 200 + i * 70, 512, 64)) {
				URender2D::SetColor(FColor(200, 200, 200, 200));
				URender2D::DrawRect(size.X / 2 - 256, 200 + i * 70, 512, 64);
			} else {
				URender2D::SetColor(FColor(133, 133, 133, 200));
				URender2D::DrawRect(size.X / 2 - 256, 200 + i * 70, 512, 64);
			}
			URender2D::SetColor(White);
			URender2D::DrawText(but->text, size.X / 2 - 256, 200 + i * 70 + 10, TEXT_ALIGN_LEFT);
		}
	}
}

void AUModHUD::DrawPlayerStats()
{
	FVector2D size = ScreenSize();

	//Variables
	uint32 Health = PlyCtrl->NativePlayer->GetHealth();
	uint32 MaxHealth = PlyCtrl->NativePlayer->GetMaxHealth();
	int Armor = 45; //Why !? There is no armor feature planned, if you want armor feature tell me.

	URender2D::SetColor(Back);
	URender2D::DrawRect(5, (size.Y / 7 * 6) - 5, size.X / 4.5, size.Y / 7);
	float BoxSizeX = size.X / 4.5;
	float BoxSizeY = size.Y / 7;
	URender2D::SetFontScale(0.5, 0.5);
	URender2D::SetColor(White);
	URender2D::DrawText("Health : " + FString::FromInt(Health), 10, (size.Y / 7 * 6) + 5, TEXT_ALIGN_LEFT);
	URender2D::SetColor(DarkGrey);
	URender2D::DrawRect(10, ((size.Y / 7 * 6) - 5) + (size.Y / 6 / 3) - 4 * 2, BoxSizeX - 10, size.Y / 7 / 5.8);
	URender2D::SetColor(White);
	URender2D::DrawText("Armor : " + FString::FromInt(Armor), 10, (size.Y / 7 * 6) + (BoxSizeY / 2), TEXT_ALIGN_LEFT);
	URender2D::SetColor(DarkGrey);
	URender2D::DrawRect(10, ((size.Y / 7 * 6) - 5) + (BoxSizeY / 4 * 3), BoxSizeX - 10, (size.Y / 7 / 5.8));

	if (Health >= MaxHealth / 2 && Health <= 100) {
		URender2D::SetColor(Green);
		URender2D::DrawRect(10, ((size.Y / 7 * 6) - 5) + (size.Y / 6 / 3) - 4 * 2, (BoxSizeX - 10) / MaxHealth * Health, size.Y / 7 / 5.8);
	} else if (Health > 20 && Health < 50) {
		URender2D::SetColor(Yellow);
		URender2D::DrawRect(10, ((size.Y / 7 * 6) - 5) + (size.Y / 6 / 3) - 4 * 2, (BoxSizeX - 10) / MaxHealth * Health, size.Y / 7 / 5.8);
	} else if (Health <= 20) {
		URender2D::SetColor(Red);
		URender2D::DrawRect(10, ((size.Y / 7 * 6) - 5) + (size.Y / 6 / 3) - 4 * 2, (BoxSizeX - 10) / MaxHealth * Health, size.Y / 7 / 5.8);
	}

	if (Armor > 100) { //Not planned, if you realy want armor management, then tell me.
		URender2D::SetColor(Green);
		URender2D::DrawRect(10, ((size.Y / 7 * 6) - 5) + (BoxSizeY / 4 * 3), BoxSizeX - 10, (size.Y / 7 / 5.8));
	} else if (Armor >= 50 && Armor <= 100) {
		URender2D::SetColor(Green);
		URender2D::DrawRect(10, ((size.Y / 7 * 6) - 5) + (BoxSizeY / 4 * 3), (BoxSizeX - 10) / 100 * Armor, (size.Y / 7 / 5.8));
	} else if (Armor > 20 && Armor < 50) {
		URender2D::SetColor(Yellow);
		URender2D::DrawRect(10, ((size.Y / 7 * 6) - 5) + (BoxSizeY / 4 * 3), (BoxSizeX - 10) / 100 * Armor, (size.Y / 7 / 5.8));
	} else if (Armor <= 20) {
		URender2D::SetColor(Red);
		URender2D::DrawRect(10, ((size.Y / 7 * 6) - 5) + (BoxSizeY / 4 * 3), (BoxSizeX - 10) / 100 * Armor, (size.Y / 7 / 5.8));
	}
}

void AUModHUD::DrawWeaponStats()
{
	FVector2D size = ScreenSize();

	//Ammo and Weapon HUD
	AWeaponBase *base = PlyCtrl->NativePlayer->GetActiveWeapon();
	if (base != NULL) {
		int Ammo = base->GetPrimaryAmmo();
		int MaxAmmo = base->GetPrimaryClipSize();
		int ReloadAmmo = PlyCtrl->NativePlayer->GetRemainingAmmo(base->GetPrimaryAmmoType());
		FString WeaponName = base->GetNiceName();
		float AmmoSizeX = size.X / 4.5;
		float AmmoSizeY = size.Y / 9;
		float AmmoPerc = 0;
		if (MaxAmmo > 0) {
			AmmoPerc = (Ammo * 100 / MaxAmmo * 100) / 100;
		}
		URender2D::SetColor(Back);
		URender2D::DrawRect((size.X / 4.5 *3.5) - 5, (size.Y / 9 * 8) - 5, size.X / 4.5, size.Y / 9);
		URender2D::SetColor(White);
		URender2D::DrawText(WeaponName, size.X / 4.5 *3.5, (size.Y / 9 * 8), TEXT_ALIGN_LEFT);
		URender2D::SetColor(DarkGrey);
		URender2D::DrawRect((size.X / 4.5 *3.5), (size.Y / 9 * 8) - 5 + AmmoSizeY / 2, AmmoSizeX - 10, AmmoSizeY / 3);
		URender2D::SetColor(AmmoCol);
		URender2D::DrawRect((size.X / 4.5 *3.5), (size.Y / 9 * 8) - 5 + AmmoSizeY / 2, ((AmmoSizeX - 10) / 100) * AmmoPerc, AmmoSizeY / 3);
		URender2D::SetColor(White);
		URender2D::DrawText(FString::FromInt(Ammo) + FString(" | ") + FString::FromInt(ReloadAmmo), size.X / 4.5 *3.5, (size.Y / 9 * 8) + AmmoSizeY / 2, TEXT_ALIGN_LEFT);
	}
}

void AUModHUD::DrawWeaponSwitch()
{
	FVector2D size = ScreenSize();

	//Weapon switch
	for (int i = 0; i < 16; i++) {
		AWeaponBase *b = PlyCtrl->NativePlayer->GetWeapons()[i];
		if (b != NULL) {
			URender2D::SetColor(FColor(0, 0, 0, 200));
			URender2D::DrawRect(size.X / 2 - (64 * 8) + i * 64, 10, 64, 64);
			URender2D::SetColor(White);
			URender2D::SetFontScale(0.2, 0.2);
			URender2D::DrawText(b->GetNiceName(), size.X / 2 - (64 * 8) + i * 64, 10, TEXT_ALIGN_LEFT);
		}
	}
}

void AUModHUD::DrawUnderwaterHUD()
{

}

void AUModHUD::MainHUDRender()
{
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	const FVector2D CrosshairDrawPosition((Center.X - (16)), (Center.Y - (16)));
	URender2D::SetTexture(CrosshairTex);
	URender2D::SetColor(FColor::White);
	URender2D::DrawRect(CrosshairDrawPosition.X, CrosshairDrawPosition.Y, 32, 32);
	URender2D::ResetTexture();

	FVector2D size = ScreenSize();

	URender2D::SetColor(FColor::Red);
	URender2D::SetFontScale(0.3, 0.3);
	/*if (PlyCtrl->Player == NULL || PlyCtrl->Player->PlayerState == NULL) {
		URender2D::DrawText("THIS CLIENT IS ERRORED", 16, 16, TEXT_ALIGN_LEFT);
	} else {
		FString txt = PlyCtrl->Player->PlayerState->PlayerName;
		URender2D::DrawText("Playing as " + txt, 16, 16, TEXT_ALIGN_LEFT);
	}*/
	FConnectionStats stats = Game->GetConnectionInfo();
	if (stats.ConnectionProblem) {
		URender2D::SetColor(FColor(255, 0, 0, 128));
		URender2D::DrawRect(0, 256, 256, 128);
		URender2D::SetColor(FColor::Black);
		URender2D::DrawText("WARNING : Connection problem !", 10, 266, TEXT_ALIGN_LEFT);
		URender2D::DrawText("Disconnect in " + FString::SanitizeFloat(stats.SecsBeforeDisconnect), 10, 286, TEXT_ALIGN_LEFT);
	}

	//Health and Armor Box
	//Yeah localPlyCL is sometimes null thanks to buggy engine !!
	if (PlyCtrl->NativePlayer != NULL) {
		DrawPlayerStats();
		DrawWeaponStats();
		DrawWeaponSwitch();
	}

	if (PlyCtrl->IsOnIngameMenu()) {
		DrawIngameMenu();
	}
}
void AUModHUD::DrawHUD()
{
	UMOD_STAT(RENDERHUD);

	if (Game == NULL || PlyCtrl == NULL) { return; }
	Super::DrawHUD();
	URender2D::SetContext(Canvas);
		
	Game->Lua->RunScriptFunctionZeroParam(ETableType::GAMEMODE, 0, "DrawHUD");

	URender2D::SetFont(HUDFont);
	MainHUDRender();
	URender2D::ExitContext();
}
