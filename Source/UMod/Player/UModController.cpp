// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "UModController.h"
#include "Game/UModGameMode.h"
#include "Game/MenuGameMode.h"
#include "Player/UModCharacter.h"

void SetKeyInConfig(FString category, FString name, FKey key)
{
	FString str = key.ToString();

	GConfig->SetString(*category, *name, *str, InputCFG);
}

void BindKeyFromConfigIfExists(FString category, FString name, FKey &key)
{
	FString str;

	GConfig->GetString(*category, *name, str, InputCFG);
	if (!str.IsEmpty()) {
		key = FKey(*str);

		UE_LOG(UMod_Input, Warning, TEXT("Found config bind for key %s : %s"), *name, *str);
	} else {
		UE_LOG(UMod_Input, Warning, TEXT("No key found in config for : %s"), *name);

		FKey def = key;
		FString str = def.ToString();

		GConfig->SetString(*category, *name, *str, InputCFG);
	}
}

/**
 * Console system destroyer (will replace by my own implementation for Lua)
 * Note to Epic : Do you know that making horrible black windows is realy easy ? I can make one in a few seconds ! A real window in an ingame menu for the console would have been much better (See GMod for more) !
 */
FString AUModController::ConsoleCommand(const FString& Cmd, bool bWriteToLog) //Make commands doing nothing
{
	return FString(); //No.
}
void AUModController::ConsoleKey(FKey Key) //Just to assure that console commannds would never be working again
{
	//Seriously I said No console
}
void AUModController::SendToConsole(const FString& Command) //Be sure nobody finds a workarroud
{
	//Realy I don't want any console
}
/**
 * End
 */

bool NeedInitialSpawn = false;

void AUModController::Tick(float f)
{
	if (GetWorld() != NULL && GetWorld()->GetAuthGameMode() != NULL && GetWorld()->GetAuthGameMode()->IsA(AMenuGameMode::StaticClass())) { return; }

	if (Role == ROLE_Authority) {
		if (GetCharacter() != NULL && NeedInitialSpawn) {
			AUModGameMode *gm = Cast<AUModGameMode>(GetWorld()->GetAuthGameMode());
			UE_LOG(UMod_Game, Log, TEXT("[DEBUG]Sending spawn notification"));
			AUModCharacter *ply = Cast<AUModCharacter>(GetCharacter());
			gm->OnPlayerInitialSpawn(ply);

			NeedInitialSpawn = false;
		}

		if (GetCharacter() == NULL) {
			AUModGameMode *gm = Cast<AUModGameMode>(GetWorld()->GetAuthGameMode());
			if (gm->CanPlayerRespawn(this)) {
				AUModCharacter *ply = GetWorld()->SpawnActor<AUModCharacter>();
				ply->SetActorLocation(GetSpawnLocation());
				ply->SetActorRotation(FRotator::ZeroRotator);
				Possess(ply);
			}
		}
	}
}

void AUModController::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld()->GetAuthGameMode()->IsA(AMenuGameMode::StaticClass())) { return; }

	if (GIsEditor) { return; }
	if (Role == ROLE_Authority) {
		UE_LOG(UMod_Game, Log, TEXT("[DEBUG]PlayerController has spawned !"))
		NeedInitialSpawn = true;
	}
}

void AUModController::InitInputSystem()
{
	Super::InitInputSystem();
	
	FKey moveForwardKey = EKeys::Z;
	FKey moveBackwardKey = EKeys::S;
	FKey moveLeftKey = EKeys::Q;
	FKey moveRightKey = EKeys::D;

	FKey primaryFireKey = EKeys::LeftMouseButton;
	FKey secondaryFireKey = EKeys::RightMouseButton;
	FKey reloadKey = EKeys::R;
	FKey useKey = EKeys::E;
	FKey jumpKey = EKeys::SpaceBar;
	FKey flashKey = EKeys::F;
	FKey weaponPrevKey = EKeys::MouseScrollDown;
	FKey weaponNextKey = EKeys::MouseScrollUp;

	BindKeyFromConfigIfExists("Movement", "FORWARD", moveForwardKey);
	BindKeyFromConfigIfExists("Movement", "BACKWARD", moveBackwardKey);
	BindKeyFromConfigIfExists("Movement", "LEFT", moveLeftKey);
	BindKeyFromConfigIfExists("Movement", "RIGHT", moveRightKey);

	BindKeyFromConfigIfExists("Actions", "JUMP", jumpKey);
	BindKeyFromConfigIfExists("Actions", "USE", useKey);
	BindKeyFromConfigIfExists("Actions", "FLASH", flashKey);
	BindKeyFromConfigIfExists("Actions", "WPREV", weaponPrevKey);
	BindKeyFromConfigIfExists("Actions", "WNEXT", weaponNextKey);

	BindKeyFromConfigIfExists("Combat", "FIRE1", primaryFireKey);
	BindKeyFromConfigIfExists("Combat", "FIRE2", secondaryFireKey);
	BindKeyFromConfigIfExists("Combat", "RELOAD", reloadKey);

	moveForward = FInputAxisKeyMapping("MoveForward", moveForwardKey, 1.0F);
	moveBackward = FInputAxisKeyMapping("MoveForward", moveBackwardKey, -1.0F);
	moveRight = FInputAxisKeyMapping("MoveRight", moveRightKey, 1.0F);
	moveLeft = FInputAxisKeyMapping("MoveRight", moveLeftKey, -1.0F);
	
	jump = FInputActionKeyMapping("Jump", jumpKey);
	use = FInputActionKeyMapping("Use", useKey);
	flash = FInputActionKeyMapping("FlashLight", flashKey);
	weaponPrev = FInputActionKeyMapping("WeaponPrev", weaponPrevKey);
	weaponNext = FInputActionKeyMapping("WeaponNext", weaponNextKey);

	primaryFire = FInputActionKeyMapping("PrimaryFire", primaryFireKey);
	secondaryFire = FInputActionKeyMapping("SecondaryFire", secondaryFireKey);
	reload = FInputActionKeyMapping("Reload", reloadKey);

	PlayerInput->AddAxisMapping(moveForward);
	PlayerInput->AddAxisMapping(moveBackward);
	PlayerInput->AddAxisMapping(moveLeft);
	PlayerInput->AddAxisMapping(moveRight);

	PlayerInput->AddActionMapping(jump);
	PlayerInput->AddActionMapping(use);
	PlayerInput->AddActionMapping(flash);
	PlayerInput->AddActionMapping(weaponPrev);
	PlayerInput->AddActionMapping(weaponNext);

	PlayerInput->AddActionMapping(primaryFire);
	PlayerInput->AddActionMapping(secondaryFire);
	PlayerInput->AddActionMapping(reload);
}

void AUModController::SetKeyBinding(EKeyBindings in, FKey newKey)
{
	switch (in) {
	case EKeyBindings::IN_LEFT:
		PlayerInput->RemoveAxisMapping(moveLeft);
		moveLeft = FInputAxisKeyMapping("MoveRight", newKey, -1.0F);
		PlayerInput->AddAxisMapping(moveLeft);
		
		SetKeyInConfig(TEXT("Movement"), TEXT("LEFT"), newKey);
		break;
	case EKeyBindings::IN_RIGHT:
		PlayerInput->RemoveAxisMapping(moveRight);
		moveRight = FInputAxisKeyMapping("MoveRight", newKey, 1.0F);
		PlayerInput->AddAxisMapping(moveRight);

		SetKeyInConfig(TEXT("Movement"), TEXT("RIGHT"), newKey);
		break;
	case EKeyBindings::IN_FORWARD:
		PlayerInput->RemoveAxisMapping(moveForward);
		moveForward = FInputAxisKeyMapping("MoveForward", newKey, 1.0F);
		PlayerInput->AddAxisMapping(moveForward);

		SetKeyInConfig(TEXT("Movement"), TEXT("FORWARD"), newKey);
		break;
	case EKeyBindings::IN_BACKWARD:
		PlayerInput->RemoveAxisMapping(moveBackward);
		moveBackward = FInputAxisKeyMapping("MoveForward", newKey, -1.0F);
		PlayerInput->AddAxisMapping(moveBackward);

		SetKeyInConfig("Movement", "BACKWARD", newKey);
		break;

	case EKeyBindings::IN_JUMP:
		PlayerInput->RemoveActionMapping(jump);
		jump = FInputActionKeyMapping("Jump", newKey);
		PlayerInput->AddActionMapping(jump);

		SetKeyInConfig("Actions", "JUMP", newKey);
		break;
	case EKeyBindings::IN_USE:
		PlayerInput->RemoveActionMapping(use);
		use = FInputActionKeyMapping("Use", newKey);
		PlayerInput->AddActionMapping(use);

		SetKeyInConfig("Actions", "USE", newKey);
		break;
	case EKeyBindings::IN_LIGHT:
		PlayerInput->RemoveActionMapping(flash);
		flash = FInputActionKeyMapping("FlashLight", newKey);
		PlayerInput->AddActionMapping(flash);

		SetKeyInConfig("Actions", "FLASH", newKey);
		break;
	case EKeyBindings::IN_WEAPON_PREV:
		PlayerInput->RemoveActionMapping(weaponPrev);
		weaponPrev = FInputActionKeyMapping("WeaponPrev", newKey);
		PlayerInput->AddActionMapping(weaponPrev);

		SetKeyInConfig("Actions", "WPREV", newKey);
		break;
	case EKeyBindings::IN_WEAPON_NEXT:
		PlayerInput->RemoveActionMapping(weaponNext);
		weaponNext = FInputActionKeyMapping("WeaponNext", newKey);
		PlayerInput->AddActionMapping(weaponNext);

		SetKeyInConfig("Actions", "WNEXT", newKey);
		break;

	case EKeyBindings::IN_FIRE1:
		PlayerInput->RemoveActionMapping(primaryFire);
		primaryFire = FInputActionKeyMapping("PrimaryFire", newKey);
		PlayerInput->AddActionMapping(primaryFire);

		SetKeyInConfig("Combat", "FIRE1", newKey);
		break;
	case EKeyBindings::IN_FIRE2:
		PlayerInput->RemoveActionMapping(secondaryFire);
		secondaryFire = FInputActionKeyMapping("SecondaryFire", newKey);
		PlayerInput->AddActionMapping(secondaryFire);

		SetKeyInConfig("Combat", "FIRE2", newKey);
		break;
	case EKeyBindings::IN_RELOAD:
		PlayerInput->RemoveActionMapping(reload);
		primaryFire = FInputActionKeyMapping("Reload", newKey);
		PlayerInput->AddActionMapping(reload);

		SetKeyInConfig("Combat", "Reload", newKey);
		break;
	}
}

FKey AUModController::LookupKeyBinding(EKeyBindings in)
{
	switch (in)
	{
	case EKeyBindings::IN_LEFT:
		return moveLeft.Key;
		break;
	case EKeyBindings::IN_RIGHT:
		return moveRight.Key;
		break;
	case EKeyBindings::IN_FORWARD:
		return moveForward.Key;
		break;
	case EKeyBindings::IN_BACKWARD:
		return moveBackward.Key;
		break;

	case EKeyBindings::IN_FIRE1:
		return primaryFire.Key;
		break;
	case EKeyBindings::IN_FIRE2:
		return secondaryFire.Key;
		break;
	case EKeyBindings::IN_RELOAD:
		return reload.Key;
		break;

	case EKeyBindings::IN_USE:
		return use.Key;
		break;
	case EKeyBindings::IN_JUMP:
		return jump.Key;
		break;
	case EKeyBindings::IN_LIGHT:
		return flash.Key;
		break;
	case EKeyBindings::IN_WEAPON_PREV:
		return weaponPrev.Key;
		break;
	case EKeyBindings::IN_WEAPON_NEXT:
		return weaponNext.Key;
		break;
	}

	return FKey();
}
