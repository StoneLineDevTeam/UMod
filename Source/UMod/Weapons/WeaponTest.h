// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "WeaponBase.h"
#include "WeaponTest.generated.h"

/**
 * 
 */
UCLASS()
class UMOD_API AWeaponTest : public AWeaponBase
{
	GENERATED_BODY()
	
	
public:
	virtual void OnPrimaryFire();
	virtual void OnSecondaryFire();
	virtual void OnReload();
	virtual void OnTick();
	virtual void OnInit();
	virtual FString GetClass();
	virtual FString GetName();
	virtual FString GetWorldModel();
	virtual FString GetViewModel();	
};
