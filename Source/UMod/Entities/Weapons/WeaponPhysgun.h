// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Entities/WeaponBase.h"
#include "WeaponPhysgun.generated.h"

class AEntityBase;

/**
 * 
 */
UCLASS()
class UMOD_API AWeaponPhysgun : public AWeaponBase
{
	GENERATED_BODY()
	
	
public:
	virtual void OnPrimaryFire(EFireState state, bool traceHit, FHitResult traceResult);
	virtual void OnSecondaryFire(EFireState state, bool traceHit, FHitResult traceResult);
	virtual void OnReload(bool traceHit, FHitResult traceResult);
	virtual void OnWeaponPrev();
	virtual void OnWeaponNext();
	virtual void OnTick();
	virtual void OnInit();
	virtual FString GetClass();
	virtual FString GetNiceName();
	virtual FString GetModel();	
	virtual EFireType GetPrimaryFireType();
	virtual EFireType GetSecondaryFireType();

private:
	class ALaserBeamRenderer *BeamEmitter;

	AEntityBase *PickedUp = NULL;
	FVector OffsetPos;
	FRotator ObjectRotation;
	
	float ObjectDistance = 100;
};
