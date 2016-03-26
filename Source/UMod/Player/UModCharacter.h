// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "Entities/WeaponBase.h"
#include "Entities/EntityBase.h"

#include "UModCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class AUModCharacter : public ACharacter
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

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface
public:
	AUModCharacter(const FObjectInitializer& ObjectInitializer);

	/* Network interface */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty, FDefaultAllocator> & OutLifetimeProps) const;
	
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;


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
	//@Shared Sets the world model for this player
	void SetModel(FString path);
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

	//Get the water level of the level
	EWaterLevel GetWaterLevel();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Damage Player", Keywords = "damage player"), Category = "UMod_Specific|Player")
		void DamagePlayer(int32 force);
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Kill Player", Keywords = "kill player"), Category = "UMod_Specific|Player")
		void KillPlayer();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Heal Player", Keywords = "heal player"), Category = "UMod_Specific|Player")
		void HealPlayer(int32 amount);

	UPROPERTY(VisibleDefaultsOnly, Category = UMod_Specific)
	class USkeletalMeshComponent* PlayerModel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UMod_Specific)
	class UCameraComponent* PlayerCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UMod_Specific)
	USpotLightComponent* SpotLight;
};

