// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "WeaponTest.h"
#include "UModCharacter.h"


void AWeaponTest::OnPrimaryFire()
{
	UE_LOG(CoreLogger, Warning, TEXT("You fired with weapon_test !"));
}

void AWeaponTest::OnSecondaryFire()
{

}

void AWeaponTest::OnReload()
{

}

void AWeaponTest::OnTick()
{

}

void AWeaponTest::OnInit()
{

}

FString AWeaponTest::GetClass()
{
	return TEXT("weapon_test");
}

FString AWeaponTest::GetName()
{
	return TEXT("Test Weapon");
}

FString AWeaponTest::GetWorldModel()
{
	return TEXT("null");
}

FString AWeaponTest::GetViewModel()
{
	return TEXT("null");
}




