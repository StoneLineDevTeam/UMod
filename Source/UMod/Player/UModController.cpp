// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "UModController.h"

void SetKeyInConfig(FString category, FString name, FKey key)
{
	FString str = key.ToString();

	GConfig->SetString(*category, *name, *str, FPaths::GameConfigDir() + FString("UMod.Input.cfg"));
}

void BindKeyFromConfigIfExists(FString category, FString name, FKey &key)
{
	FString str;

	GConfig->GetString(*category, *name, str, FPaths::GameConfigDir() + FString("UMod.Input.cfg"));
	if (!str.IsEmpty()) {
		key = FKey(*str);

		UE_LOG(UMod_Input, Warning, TEXT("Found config bind for key %s : %s"), *name, *str);
	} else {
		UE_LOG(UMod_Input, Warning, TEXT("No key found in config for : %s"), *name);

		FKey def = key;
		FString str = def.ToString();

		GConfig->SetString(*category, *name, *str, FPaths::GameConfigDir() + FString("UMod.Input.cfg"));
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

	BindKeyFromConfigIfExists(TEXT("Movement"), TEXT("FORWARD"), moveForwardKey);
	BindKeyFromConfigIfExists(TEXT("Movement"), TEXT("BACKWARD"), moveBackwardKey);
	BindKeyFromConfigIfExists(TEXT("Movement"), TEXT("LEFT"), moveLeftKey);
	BindKeyFromConfigIfExists(TEXT("Movement"), TEXT("RIGHT"), moveRightKey);

	BindKeyFromConfigIfExists(TEXT("Actions"), TEXT("JUMP"), jumpKey);
	BindKeyFromConfigIfExists(TEXT("Actions"), TEXT("USE"), useKey);

	BindKeyFromConfigIfExists(TEXT("Combat"), TEXT("FIRE1"), primaryFireKey);
	BindKeyFromConfigIfExists(TEXT("Combat"), TEXT("FIRE2"), secondaryFireKey);
	BindKeyFromConfigIfExists(TEXT("Combat"), TEXT("RELOAD"), reloadKey);

	moveForward = FInputAxisKeyMapping("MoveForward", moveForwardKey, 1.0F);
	moveBackward = FInputAxisKeyMapping("MoveForward", moveBackwardKey, -1.0F);
	moveRight = FInputAxisKeyMapping("MoveRight", moveRightKey, 1.0F);
	moveLeft = FInputAxisKeyMapping("MoveRight", moveLeftKey, -1.0F);
	
	jump = FInputActionKeyMapping("Jump", jumpKey);
	use = FInputActionKeyMapping("Use", useKey);

	primaryFire = FInputActionKeyMapping("PrimaryFire", primaryFireKey);
	secondaryFire = FInputActionKeyMapping("SecondaryFire", secondaryFireKey);
	reload = FInputActionKeyMapping("Reload", reloadKey);

	PlayerInput->AddAxisMapping(moveForward);
	PlayerInput->AddAxisMapping(moveBackward);
	PlayerInput->AddAxisMapping(moveLeft);
	PlayerInput->AddAxisMapping(moveRight);

	PlayerInput->AddActionMapping(jump);
	PlayerInput->AddActionMapping(use);

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

		SetKeyInConfig(TEXT("Movement"), TEXT("BACKWARD"), newKey);
		break;

	case EKeyBindings::IN_JUMP:
		PlayerInput->RemoveActionMapping(jump);
		jump = FInputActionKeyMapping("Jump", newKey);
		PlayerInput->AddActionMapping(jump);

		SetKeyInConfig(TEXT("Actions"), TEXT("JUMP"), newKey);
		break;
	case EKeyBindings::IN_USE:
		PlayerInput->RemoveActionMapping(use);
		use = FInputActionKeyMapping("Use", newKey);
		PlayerInput->AddActionMapping(use);

		SetKeyInConfig(TEXT("Actions"), TEXT("USE"), newKey);
		break;

	case EKeyBindings::IN_FIRE1:
		PlayerInput->RemoveActionMapping(primaryFire);
		primaryFire = FInputActionKeyMapping("PrimaryFire", newKey);
		PlayerInput->AddActionMapping(primaryFire);

		SetKeyInConfig(TEXT("Combat"), TEXT("FIRE1"), newKey);
		break;
	case EKeyBindings::IN_FIRE2:
		PlayerInput->RemoveActionMapping(secondaryFire);
		secondaryFire = FInputActionKeyMapping("SecondaryFire", newKey);
		PlayerInput->AddActionMapping(secondaryFire);

		SetKeyInConfig(TEXT("Combat"), TEXT("FIRE2"), newKey);
		break;
	case EKeyBindings::IN_RELOAD:
		PlayerInput->RemoveActionMapping(reload);
		primaryFire = FInputActionKeyMapping("Reload", newKey);
		PlayerInput->AddActionMapping(reload);

		SetKeyInConfig(TEXT("Combat"), TEXT("Reload"), newKey);
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
	}

	return FKey();
}
