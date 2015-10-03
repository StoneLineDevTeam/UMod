// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "WeaponTest.h"
#include "Player/UModCharacter.h"

bool ActorHasMesh(AActor *act)
{
	TArray<UStaticMeshComponent*> comps;
	act->GetComponents<UStaticMeshComponent>(comps);

	return comps.Num() > 0;
}

void EnableGravityOnActor(AActor *act, bool enable)
{
	TArray<UStaticMeshComponent*> comps;
	act->GetComponents<UStaticMeshComponent>(comps);

	for (int i = 0; i < comps.Num(); i++) {
		UStaticMeshComponent *c = comps[i];
		c->SetSimulatePhysics(enable);
	}
}

bool IsActorMovable(AActor *act)
{
	TArray<UStaticMeshComponent*> comps;
	act->GetComponents<UStaticMeshComponent>(comps);

	bool b = true;

	for (int i = 0; i < comps.Num(); i++) {
		UStaticMeshComponent *c = comps[i];		
		if (c->Mobility == EComponentMobility::Static) {
			b = false;
		}
	}

	return b;
}

void EnableMobilityOnActor(AActor *act, bool enable)
{
	TArray<UStaticMeshComponent*> comps;
	act->GetComponents<UStaticMeshComponent>(comps);

	for (int i = 0; i < comps.Num(); i++) {
		UStaticMeshComponent *c = comps[i];
		if (enable) {
			c->SetMobility(EComponentMobility::Movable);
		} else {
			c->SetMobility(EComponentMobility::Static);
		}
	}
}

void AWeaponTest::OnPrimaryFire(AWeaponBase::EFireState state, bool traceHit, FHitResult traceResult)
{
	if (state == AWeaponBase::EFireState::ENDED && PickedUp != NULL) {
		UE_LOG(UMod_Game, Warning, TEXT("You stopped to fire with weapon_test !"));
		EnableGravityOnActor(PickedUp, true);
		PickedUp = NULL;
		OffsetPos = FVector(0, 0, 0);
		ObjectDistance = 100;
		return;
	}

	if (state == AWeaponBase::EFireState::STARTED && traceHit) {
		UE_LOG(UMod_Game, Warning, TEXT("You started to fire with weapon_test !"));
		AActor *act = traceResult.GetActor();
		if (!ActorHasMesh(act) || !IsActorMovable(act)) {
			return;
		}		
		PickedUp = act;		
		EnableGravityOnActor(PickedUp, false);
		OffsetPos = player->PlayerCamera->GetComponentLocation() - act->GetActorLocation();
		ObjectDistance = FVector::Dist(player->GetEyeLocation(), act->GetActorLocation());
	}

	if (state == AWeaponBase::EFireState::FIRING && PickedUp != NULL) {
		FRotator rot = PickedUp->GetActorRotation();
		FVector pos = player->GetEyeLocation() + (player->GetEyeAngles().Vector() * ObjectDistance);
		PickedUp->SetActorLocation(pos);
		PickedUp->SetActorRotation(rot);
	}
}

void AWeaponTest::OnSecondaryFire(AWeaponBase::EFireState state, bool traceHit, FHitResult traceResult)
{

}

void AWeaponTest::OnReload(bool traceHit, FHitResult traceResult)
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

FString AWeaponTest::GetNiceName()
{
	return TEXT("Test Weapon");
}

FString AWeaponTest::GetModel()
{
	return TEXT("PhysicsGun");
}

AWeaponBase::EFireType AWeaponTest::GetPrimaryFireType()
{
	return AWeaponBase::EFireType::CONTINUES;
}

AWeaponBase::EFireType AWeaponTest::GetSecondaryFireType()
{
	return AWeaponBase::EFireType::SIMPLE;
}