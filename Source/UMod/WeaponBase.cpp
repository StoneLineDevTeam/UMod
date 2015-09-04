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

	this->OnInit();
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	this->OnTick();
}

void AWeaponBase::DoInit(AUModCharacter *ply)
{
	player = ply;
}

void AWeaponBase::OnPlayerFire(uint8 but)
{
	//Using this wrapping function in case we want to add code before / after fire (like animation)
	if (but == 0) {
		this->OnPrimaryFire();
	} else if (but == 1) {
		this->OnSecondaryFire();
	} else if (but == 2) {
		this->OnReload();
	}
}

void AWeaponBase::OnPrimaryFire()
{

}

void AWeaponBase::OnSecondaryFire()
{

}

void AWeaponBase::OnReload()
{

}

void AWeaponBase::OnInit()
{

}

void AWeaponBase::OnTick()
{

}

FString AWeaponBase::GetClass()
{
	return TEXT("weapon_null");
}

FString AWeaponBase::GetWorldModel()
{
	return TEXT("null");
}

FString AWeaponBase::GetViewModel()
{
	return TEXT("null");
}