// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "UModController.generated.h"


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
class UMOD_API AUModController : public APlayerController
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

	virtual void Tick(float f);
private:
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
};
