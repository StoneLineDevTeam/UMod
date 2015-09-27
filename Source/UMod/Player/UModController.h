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
	IN_JUMP
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
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set KeyBinding", Keywords = "key binding set"), Category = UMod_Specific)
		void SetKeyBinding(EKeyBindings in, FKey newKey);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get KeyBinding", Keywords = "key binding get"), Category = UMod_Specific)
		FKey LookupKeyBinding(EKeyBindings in);

private:
	FInputAxisKeyMapping moveForward;
	FInputAxisKeyMapping moveBackward;
	FInputAxisKeyMapping moveRight;
	FInputAxisKeyMapping moveLeft;

	FInputActionKeyMapping jump;
	FInputActionKeyMapping use;

	FInputActionKeyMapping primaryFire;
	FInputActionKeyMapping secondaryFire;
	FInputActionKeyMapping reload;
};
