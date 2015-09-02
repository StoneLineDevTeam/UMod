// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "UModCharacter.h"
#include "WeaponBase.generated.h"

UCLASS()
class UMOD_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	
	virtual void OnPlayerFire(uint8 but, AUModCharacter player);

	void OnPrimaryFire(AUModCharacter player);
	void OnSecondaryFire(AUModCharacter player);
	void OnReload(AUModCharacter player);
	FString GetClass();
	FString GetName();	
	FString GetModel();
};
