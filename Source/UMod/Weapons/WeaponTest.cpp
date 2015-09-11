// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "WeaponTest.h"
#include "UModCharacter.h"


void AWeaponTest::OnPrimaryFire()
{
	UE_LOG(UMod_Game, Warning, TEXT("You fired with weapon_test !"));

	/* Will switch that in a test weapon when done the attachement system and network system
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
	const FRotator SpawnRotation = GetControlRotation();
	// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(GunOffset);

	UWorld* const World = GetWorld();
	if (World != NULL)
	{
	// spawn the projectile at the muzzle
	World->SpawnActor<AUModProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
	}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if(FireAnimation != NULL)
	{
	// Get the animation object for the arms mesh
	UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
	if(AnimInstance != NULL)
	{
	AnimInstance->Montage_Play(FireAnimation, 1.f);
	}
	}*/
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




