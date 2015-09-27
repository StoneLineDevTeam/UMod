// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
//#include "UModCharacter.h"
#include "WeaponBase.generated.h"

class AUModCharacter;

UCLASS()
class UMOD_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
	class UStaticMeshComponent* ViewModel;
	class UStaticMeshComponent* WorldModel;
public:

	enum EFireType {
		CONTINUES,
		SIMPLE
	};

	enum EFireState {
		STARTED,
		FIRING,
		ENDED
	};

	// Sets default values for this actor's properties
	AWeaponBase();
	
	//The player on which weapon is attached to
	AUModCharacter *player;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
	
	void OnPlayerFire(uint8 but, EFireState state);
		
	virtual void OnPrimaryFire(EFireState state, bool traceHit, FHitResult traceResult);
	virtual void OnSecondaryFire(EFireState state, bool traceHit, FHitResult traceResult);
	virtual void OnReload(bool traceHit, FHitResult traceResult);
	virtual void OnTick();
	virtual void OnInit();
	virtual FVector GetGunOffset();
	virtual FString GetClass();
	virtual FString GetNiceName();
	virtual FString GetModel();
	virtual EFireType GetPrimaryFireType();
	virtual EFireType GetSecondaryFireType();

	/* Internal methods don't call them */
	void DoInit(AUModCharacter *ply);
	void Equip();
	void UnEquip();
	/* End */	
};
