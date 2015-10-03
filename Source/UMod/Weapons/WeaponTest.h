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
	virtual void OnPrimaryFire(EFireState state, bool traceHit, FHitResult traceResult);
	virtual void OnSecondaryFire(EFireState state, bool traceHit, FHitResult traceResult);
	virtual void OnReload(bool traceHit, FHitResult traceResult);
	virtual void OnTick();
	virtual void OnInit();
	virtual FString GetClass();
	virtual FString GetNiceName();
	virtual FString GetModel();	
	virtual EFireType GetPrimaryFireType();
	virtual EFireType GetSecondaryFireType();

private:
	AActor *PickedUp = NULL;
	FVector OffsetPos;
	
	float ObjectDistance = 100;
};
