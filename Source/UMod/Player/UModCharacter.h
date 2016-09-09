// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "Entities/WeaponBase.h"
#include "Entities/EntityBase.h"
#include "Entities/Entity.h"

#include "UModCharacter.generated.h"

class UInputComponent;

UCLASS()
class AUModCharacter : public ACharacter, public Entity
{
	GENERATED_BODY()
			
private:
	//The weapon matrix which stores all pointers to all AActors corresponding to those weapons
	UPROPERTY(ReplicatedUsing = UpdateClientSideData)
	AWeaponBase *weapons[16];
	//The current weapon slot id which corresponds to the weapon the player holds
	UPROPERTY(ReplicatedUsing = UpdateClientSideData)
	uint8 curWeapon;
	//The ammo the player currently has
	UPROPERTY(ReplicatedUsing = UpdateClientSideData)
	TMap<FString, uint32> PlayerAmmo;

	//Function to be called right after some vars have been replicated.
	UFUNCTION()
	void UpdateClientSideData();

	UFUNCTION(Server, UnReliable, WithValidation)
	void OnPlayerClick(uint8 but);
	UFUNCTION(Server, UnReliable, WithValidation)
	void OnPlayerSpecialKey(uint8 bind, bool pressed);

	void OnPlayerClick_Implementation(uint8 but);
	bool OnPlayerClick_Validate(uint8 but);
	void OnPlayerSpecialKey_Implementation(uint8 bind, bool pressed);
	bool OnPlayerSpecialKey_Validate(uint8 bind, bool pressed);

	UPROPERTY(ReplicatedUsing = UpdateClientSideData)
	uint32 playerHealth;
	
	UPROPERTY(ReplicatedUsing = UpdateClientSideData)
	uint32 playerMaxHealth;

	bool InFire1;
	bool InFire2;

	bool UseKey;

	int LuaReference;
	FString LuaClassName;

	/* Tried to do this in macros however it seam that UBT runs UHT before C++ preprocessor... */
	UPROPERTY(Replicated)
	TMap<FString, int> NWInts;
	UPROPERTY(Replicated)
	TMap<FString, FString> NWStrings;
	UPROPERTY(Replicated)
	TMap<FString, uint32> NWUInts;
	UPROPERTY(Replicated)
	TMap<FString, float> NWFloats;
protected:
	
	/* Fire system */
	void StartPrimaryFire();
	void EndPrimaryFire();
	void StartSecondaryFire();
	void EndSecondaryFire();
	void HandleReload();

	/* Special actions */
	void HandleUseStart();
	void HandleUseEnd();
	void HandleFlashLight();
	void HandleWeaponPrev();
	void HandleWeaponNext();
		
	/* Handles moving forward/backward */
	void MoveForward(float Val);

	/* Handles stafing movement, left and right */
	void MoveRight(float Val);

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface
public:
	AUModCharacter(const FObjectInitializer& ObjectInitializer);

	/* Network interface */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty, FDefaultAllocator> & OutLifetimeProps) const;
	
	virtual void BeginPlay();
	virtual void EndPlay(EEndPlayReason::Type reason);
	
	virtual void Tick(float DeltaSeconds) override;

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult);

	/* UMod Specific */

	//@ServerSide Gives a weapon to the player
	void GiveWeapon(UClass* cl);
	//@ServerSide Removes the current weapon the player holds
	void RemoveActiveWeapon();
	//@ServerSide Removes all weapons this player is holding
	void StripWeapons();
	//@Shared Returns all weapons
	AWeaponBase** GetWeapons();
	//@Shared returns the weapon the player holds
	AWeaponBase* GetActiveWeapon();
	//@ServerSide Switchs to another weapon slot
	void SwitchWeapon(uint8 id);
	//@ServerSide gives ammo (does not reload weapon clip !)
	void GiveAmmo(FString ammoType, uint32 amount);
	void RemoveAmmo(FString ammoType, uint32 amount);
	FVector GetEyeLocation();
	FRotator GetEyeAngles();
	FVector GetRightHandLocation();
	FVector GetLeftHandLocation();
	uint32 GetRemainingAmmo(FString ammoType);
	bool IsUseKeyDown();
	
	//Health system
	void SetHealth(int32 var);
	void SetMaxHealth(uint32 var);
	uint32 GetHealth();
	uint32 GetMaxHealth();
	//End

	void DamagePlayer(int32 force);
	
	void KillPlayer();

	void HealPlayer(int32 amount);

	/*Entity Inetrface start*/
	//Lua
	virtual int GetLuaRef();
	virtual void SetLuaRef(int r);
	virtual void LuaUnRef();
	virtual void SetLuaClass(FString s);

	virtual void Remove();
	virtual int EntIndex();
	virtual void SetPos(FVector vec);
	virtual void SetAngles(FRotator ang);
	virtual FVector GetPos();
	virtual FRotator GetAngles();
	virtual void SetColor(FColor col);
	virtual FColor GetColor();

	//lib
	virtual void AddPhysicsObject(); //Works only in OnInit(), used to remove the synced physics system, ServerSide
	virtual FPhysObj *GetPhysicsObject();
	virtual void SetCollisionModel(ECollisionType collision); //ServerSide
	virtual ECollisionType GetCollisionModel();
	virtual void SetModel(FString path);
	virtual FString GetModel();
	virtual void SetMaterial(FString path); //No sync possible : UE4 does not allow c array replication
	virtual void SetSubMaterial(int32 index, FString path); //No sync possible : UE4 does not allow c array replication
	virtual FString GetMaterial();
	virtual FString GetSubMaterial(int32 index);
	virtual int32 GetSubMaterialsNum();
	virtual FString GetClass();
	//NW Vars (Future) (Using spacial macro)
	AUTO_NWVARS_HEADER();
	virtual EWaterLevel GetWaterLevel();
	/* Entity Interface end */

	UPROPERTY(VisibleDefaultsOnly, Category = UMod_Specific)
	class USkeletalMeshComponent* PlayerModel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UMod_Specific)
	class UCameraComponent* PlayerCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UMod_Specific)
	class USceneComponent *CameraHack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UMod_Specific)
	USpotLightComponent* SpotLight;
};

