// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
//#include "UModCharacter.h"
#include "WeaponBase.generated.h"

class AUModCharacter;

UCLASS()
class AWeaponBase : public AActor
{
	GENERATED_BODY()
	
	class UStaticMeshComponent* ViewModel;
	class UStaticMeshComponent* WorldModel;

private:
	UPROPERTY(ReplicatedUsing = UpdateClientModels)
	bool ModelEquipped;

	UFUNCTION()
	void UpdateClientModels();

	bool Initialized;

	uint32 CurPrimaryClipSize;
	uint32 CurSecondaryClipSize;

	class USoundWave* FireSound;
	class UAudioComponent *AudioComp;
public:

	static const FString NULLAmmoType;

	enum EFireType {
		CONTINUES,
		SIMPLE
	};

	enum EFireState {
		STARTED,
		FIRING,
		ENDED
	};

	enum EWeaponAttachPos {
		LEFT_HAND,
		RIGHT_HAND,
		BODY_BACK,
		BOTH_HANDS
	};

	enum EClipType {
		PRIMARY,
		SECONDARY
	};

	/* Weapon interface */
	AUModCharacter *Player;
	UUModGameInstance *Game;
	virtual void OnPrimaryFire(EFireState state, bool traceHit, FHitResult traceResult);
	virtual void OnSecondaryFire(EFireState state, bool traceHit, FHitResult traceResult);
	virtual void OnReload(bool traceHit, FHitResult traceResult);
	virtual void OnWeaponPrev();
	virtual void OnWeaponNext();
	virtual void OnTick();
	virtual void OnInit();
	virtual FVector GetGunOffset();
	virtual FString GetClass();
	virtual FString GetNiceName();
	virtual FString GetWorldModel();
	virtual FString GetViewModel();
	virtual EFireType GetPrimaryFireType();
	virtual EFireType GetSecondaryFireType();
	virtual FString GetPrimaryAmmoType();
	virtual FString GetSecondaryAmmoType();
	virtual uint32 GetPrimaryClipSize();
	virtual uint32 GetSecondaryClipSize();
	virtual FString GetFireSoundName();
	virtual bool CanReloadClip(EClipType clip);
	/* End */

	/* Weapons utility */
	void SetAttachLocation(EWeaponAttachPos newPos);
	//Consumes ammo of clip, returns false if clip has not enough ammo
	bool ConsumeAmmo(EClipType clip, uint32 amount);
	//Reloads clip, returns false if player has not enough ammo for reload
	bool ReloadClip(EClipType clip);
	//Broadcasts the fire sound to all connected clients
	void BroadcastFireSound();
	uint32 GetPrimaryAmmo();
	uint32 GetSecondaryAmmo();
	/* End */

	/* Internal methods don't call them */
	void DoInit(AUModCharacter *ply);
	void Equip();
	void UnEquip();
	void ClientInit(AUModCharacter *ply);
	void OnPlayerFire(uint8 but, EFireState state);
	// Actor interface
	AWeaponBase();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty, FDefaultAllocator> & OutLifetimeProps) const;
	UFUNCTION(NetMulticast, Unreliable)
	void RunFireSound();
	void RunFireSound_Implementation();
	/* End */
};
