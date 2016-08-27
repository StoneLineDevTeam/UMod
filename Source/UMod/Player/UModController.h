// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "UModController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FControllerOnMouseClick, float, x, float, y);

UENUM(BlueprintType)
enum class EKeyBindings {
	IN_LEFT,
	IN_RIGHT,
	IN_FORWARD,
	IN_BACKWARD,

	IN_FIRE1,
	IN_FIRE2,
	IN_RELOAD,

	IN_USE,
	IN_JUMP,
	IN_WEAPON_PREV,
	IN_WEAPON_NEXT,
	IN_LIGHT
};

/**
 * 
 */
UCLASS()
class AUModController : public APlayerController
{
	GENERATED_BODY()
		
public:
	virtual void InitInputSystem();
	virtual void BeginPlay();
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set KeyBinding", Keywords = "key binding set"), Category = "UMod_Specific|Player")
	void SetKeyBinding(EKeyBindings in, FKey newKey);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get KeyBinding", Keywords = "key binding get"), Category = "UMod_Specific|Player")
	FKey LookupKeyBinding(EKeyBindings in);

	//Console destroy
	virtual FString ConsoleCommand(const FString& Cmd, bool bWriteToLog);
	virtual void ConsoleKey(FKey Key);
	virtual void SendToConsole(const FString& Command);
	//End

	virtual bool InputKey(FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad);

	virtual void Tick(float f);

	void EnterIngameMenu();
	void ExitIngameMenu();

	bool IsOnIngameMenu();

	void EnterMenu();
	void ExitMenu();

	AUModCharacter *Player;

	FControllerOnMouseClick OnMouseClick;
private:
	bool NeedInitialSpawn;
	
	FInputAxisKeyMapping moveForward;
	FInputAxisKeyMapping moveBackward;
	FInputAxisKeyMapping moveRight;
	FInputAxisKeyMapping moveLeft;

	FInputActionKeyMapping jump;
	FInputActionKeyMapping use;
	FInputActionKeyMapping flash;
	FInputActionKeyMapping weaponPrev;
	FInputActionKeyMapping weaponNext;

	FInputActionKeyMapping primaryFire;
	FInputActionKeyMapping secondaryFire;
	FInputActionKeyMapping reload;
	
	bool InMenu;
	bool ShouldDrawIngameMenu = false;

	bool IsIngameMenuUnavailable = false; //FUTURE : Implementation of lua custom ingame menu
};
