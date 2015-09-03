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
	
public:
	// Sets default values for this actor's properties
	AWeaponBase();
	
	//The player on which weapon is attached to
	AUModCharacter *player;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	
	virtual void OnPlayerFire(uint8 but);

	virtual void DoInit(AUModCharacter *ply);
	
	void OnPrimaryFire();
	void OnSecondaryFire();
	void OnReload();
	void OnTick();
	void OnInit();
	FString GetClass();
	FString GetName();	
	FString GetWorldModel();
	FString GetViewModel();
};
