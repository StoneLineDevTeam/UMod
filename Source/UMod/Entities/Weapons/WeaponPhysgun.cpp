// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "WeaponPhysgun.h"
#include "Player/UModCharacter.h"
#include "Entities/EntityBase.h"

#include "Renderer/LaserBeamRenderer.h"

bool ActorHasMesh(AActor *act)
{
	TArray<UStaticMeshComponent*> comps;
	act->GetComponents<UStaticMeshComponent>(comps);

	return comps.Num() > 0;
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

void AWeaponPhysgun::OnPrimaryFire(AWeaponBase::EFireState state, bool traceHit, FHitResult traceResult)
{
	if (state == AWeaponBase::EFireState::ENDED && PickedUp != NULL) {
		BeamEmitter->Destroy();

		UE_LOG(UMod_Game, Warning, TEXT("You stopped to fire with the physics gun !"));
		PickedUp->GetPhysicsObject()->UnFreeze();
		PickedUp = NULL;
		OffsetPos = FVector(0, 0, 0);
		ObjectDistance = 100;
		return;
	}

	if (state == AWeaponBase::EFireState::STARTED && traceHit) {		
		UE_LOG(UMod_Game, Warning, TEXT("You started to fire with the physics gun !"));
		AActor *act = traceResult.GetActor();		
		if (!act->GetClass()->IsChildOf(AEntityBase::StaticClass())) {
			return;
		}
		PickedUp = Cast<AEntityBase>(act);
		PickedUp->GetPhysicsObject()->Freeze();
		OffsetPos = act->GetActorLocation() - traceResult.ImpactPoint;
		ObjectDistance = FVector::Dist(Player->GetEyeLocation(), traceResult.ImpactPoint);
		ObjectRotation = act->GetActorRotation();

		BeamEmitter = ALaserBeamRenderer::CreateBeamRenderer(GetWorld(), this->GetActorLocation(), PickedUp->GetActorLocation());
	}

	if (state == AWeaponBase::EFireState::FIRING && PickedUp != NULL) {
		FVector pos = (Player->GetEyeLocation() + OffsetPos) + (Player->GetEyeAngles().Vector() * ObjectDistance);
		PickedUp->SetActorLocation(pos);
		PickedUp->SetActorRotation(ObjectRotation);
		
		if (BeamEmitter != NULL) {
			BeamEmitter->UpdateVectors(Player->GetRightHandLocation(), pos);
		}
		if (Player->IsUseKeyDown()) {
			ObjectRotation = Player->GetEyeAngles();
		}
	}
}

void AWeaponPhysgun::OnSecondaryFire(AWeaponBase::EFireState state, bool traceHit, FHitResult traceResult)
{
	if (traceHit) {
		AActor* act = traceResult.GetActor();
		if (act->GetClass()->IsChildOf(AEntityBase::StaticClass())) {
			AEntityBase *b = Cast<AEntityBase>(act);
			b->GetPhysicsObject()->Freeze();
		}
	}	
}

void AWeaponPhysgun::OnWeaponPrev()
{
	if (PickedUp != NULL) {
		ObjectDistance -= 25;
	}
}

void AWeaponPhysgun::OnWeaponNext()
{
	if (PickedUp != NULL) {
		ObjectDistance += 25;
	}
}

void AWeaponPhysgun::OnReload(bool traceHit, FHitResult traceResult)
{

}

void AWeaponPhysgun::OnInit()
{

}

FString AWeaponPhysgun::GetClass()
{
	return TEXT("weapon_physgun");
}

FString AWeaponPhysgun::GetNiceName()
{
	return TEXT("PHYSICS GUN");
}

FString AWeaponPhysgun::GetWorldModel()
{
	return TEXT("UMod:Weapons/W_PhysicsGun");
}

FString AWeaponPhysgun::GetViewModel()
{
	return TEXT("UMod:Weapons/V_PhysicsGun");
}

AWeaponBase::EFireType AWeaponPhysgun::GetPrimaryFireType()
{
	return AWeaponBase::EFireType::CONTINUES;
}

AWeaponBase::EFireType AWeaponPhysgun::GetSecondaryFireType()
{
	return AWeaponBase::EFireType::SIMPLE;
}