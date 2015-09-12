// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "WeaponBase.h"
#include "UModCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class AUModCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* PlayerModel;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* PlayerCamera;
private:
	/** Gun muzzle's offset from the characters location */
	FVector GunOffset;

	/** Sound to play each time we fire */
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	class UAnimMontage* FireAnimation;



	/*UMod Specific*/

	//The weapon matrix which stores all pointers to all AActors corresponding to those weapons
	UPROPERTY(ReplicatedUsing = UpdateClientSideData)
		AWeaponBase *weapons[16];
	//The current weapon slot id which corresponds to the weapon the player holds
	UPROPERTY(ReplicatedUsing = UpdateClientSideData)
		uint8 curWeapon;
	//This function is used to create an attachement between player and weapon
	virtual void UpdateAttachement();

	//Function to be called right after some vars have been replicated.
	UFUNCTION()
		void UpdateClientSideData();

	UFUNCTION(Server, UnReliable, WithValidation)
		void OnPlayerClick(uint8 but);

	void OnPlayerClick_Implementation(uint8 but);
	bool OnPlayerClick_Validate(uint8 but);

	UPROPERTY(ReplicatedUsing = UpdateClientSideData)
		uint32 playerHealth;
	
	UPROPERTY(ReplicatedUsing = UpdateClientSideData)
		uint32 playerMaxHealth;
protected:
	
	/** Fires a projectile. */
	void OnFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
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

	void HandleUse();
	
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface
public:
	AUModCharacter(const FObjectInitializer& ObjectInitializer);

	/* Network interface */
	virtual void AUModCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty, FDefaultAllocator> & OutLifetimeProps) const;
	


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
	
	//Health system
	void SetHealth(int32 var);

	void SetMaxHealth(uint32 var);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Damage Player", Keywords = "damage player"), Category = UMod_Specific)
		void DamagePlayer(int32 force);
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Kill Player", Keywords = "kill player"), Category = UMod_Specific)
		void KillPlayer();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Heal Player", Keywords = "heal player"), Category = UMod_Specific)
		void HealPlayer(int32 amount);

	uint32 GetHealth();
	uint32 GetMaxHealth();
};

