// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "WeaponBase.h"


// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponBase::OnPlayerFire(uint8 but, AUModCharacter player)
{

}

FString AWeaponBase::GetClass()
{
	return TEXT("weapon_null");
}

FString AWeaponBase::GetModel()
{
	return TEXT("weapon_null");
}

