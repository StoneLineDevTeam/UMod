// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "WeaponBase.h"
#include "UModGameInstance.h"
#include "Sound/SoundWave.h"

#include "Player/UModCharacter.h"

const FString AWeaponBase::NULLAmmoType = FString();

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent *RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	ViewModel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ViewModel"));	
	WorldModel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WorldModel"));

	SetRootComponent(RootComponent);

	//ViewModel->SetVisibility(false, true);
	//WorldModel->SetVisibility(false, true);

	ViewModel->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WorldModel->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	bReplicates = true;

	if (GetFireSoundName() != "null") {
		FString assetPath = "/Game/Sounds/Weapons/" + GetFireSoundName();
		FireSound = LoadObjFromPath<USoundWave>(*assetPath);
		AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
		AudioComp->SetSound(FireSound);
	}
}

void AWeaponBase::ClientInit(AUModCharacter *ply)
{
	if (!Initialized) {
		if (!this->GetModel().IsEmpty() && !this->GetModel().Equals("null")) {
			FString vMdlPath = FString("/Game/Models/Weapons/V_") + this->GetModel();
			FString wMdlPath = FString("/Game/Models/Weapons/W_") + this->GetModel();
			UStaticMesh *vMesh = LoadObjFromPath<UStaticMesh>(*vMdlPath);
			UStaticMesh *wMesh = LoadObjFromPath<UStaticMesh>(*wMdlPath);

			ViewModel->SetStaticMesh(vMesh);
			ViewModel->SetOnlyOwnerSee(true);

			WorldModel->SetStaticMesh(wMesh);
			WorldModel->SetOwnerNoSee(true);
		}

		player = ply;
		SetActorLocation(ply->GetRightHandLocation() + GetGunOffset());
		AttachRootComponentToActor(ply);

		WorldModel->AttachTo(ply->PlayerModel, "RHand_AttachPoint", EAttachLocation::SnapToTarget);

		Initialized = true;
	}
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

	this->OnTick();
}

void AWeaponBase::DoInit(AUModCharacter *ply)
{
	if (!this->GetModel().IsEmpty() && !this->GetModel().Equals("null")) {
		FString vMdlPath = FString("/Game/Models/Weapons/V_") + this->GetModel();
		FString wMdlPath = FString("/Game/Models/Weapons/W_") + this->GetModel();
		UStaticMesh *vMesh = LoadObjFromPath<UStaticMesh>(*vMdlPath);
		UStaticMesh *wMesh = LoadObjFromPath<UStaticMesh>(*wMdlPath);

		ViewModel->SetStaticMesh(vMesh);
		ViewModel->SetOnlyOwnerSee(true);

		WorldModel->SetStaticMesh(wMesh);
		WorldModel->SetOwnerNoSee(true);
	}

	player = ply;
	SetActorLocation(ply->GetRightHandLocation() + GetGunOffset());
	AttachRootComponentToActor(ply);

	WorldModel->AttachTo(ply->PlayerModel, "RHand_AttachPoint", EAttachLocation::SnapToTarget);

	this->OnInit();
}

void AWeaponBase::UpdateClientModels()
{	
	if (ModelEquipped) {
		WorldModel->SetVisibility(true, true);
		ViewModel->SetVisibility(true, true);
	} else {
		ViewModel->SetVisibility(false, true);
		WorldModel->SetVisibility(false, true);
	}
}

void AWeaponBase::Equip()
{
	WorldModel->SetVisibility(true, true);
	ViewModel->SetVisibility(true, true);
	ModelEquipped = true;
}

void AWeaponBase::UnEquip()
{
	ViewModel->SetVisibility(false, true);
	WorldModel->SetVisibility(false, true);
	ModelEquipped = false;
}

void AWeaponBase::OnPlayerFire(uint8 but, AWeaponBase::EFireState state)
{
	if (player == NULL) {
		return;
	}

	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.AddIgnoredActor(player);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;

	FHitResult result(ForceInit);
	FVector p = player->GetEyeLocation();
	FVector rot = player->GetEyeAngles().Vector();
	bool hit = GetWorld()->LineTraceSingleByChannel(result,	p, p + (rot * 2000), ECC_Pawn, RV_TraceParams);
	
	//Using this wrapping function in case we want to add code before / after fire (like animation)
	if (but == 0) {
		this->OnPrimaryFire(state, hit, result);
	} else if (but == 1) {
		this->OnSecondaryFire(state, hit, result);
	} else if (but == 2) {
		this->OnReload(hit, result);
	}
}

void AWeaponBase::OnPrimaryFire(AWeaponBase::EFireState state, bool traceHit, FHitResult traceResult)
{

}

void AWeaponBase::OnSecondaryFire(AWeaponBase::EFireState state, bool traceHit, FHitResult traceResult)
{

}

void AWeaponBase::OnReload(bool traceHit, FHitResult traceResult)
{

}

void AWeaponBase::OnWeaponPrev()
{

}

void AWeaponBase::OnWeaponNext()
{

}

void AWeaponBase::OnInit()
{

}

void AWeaponBase::OnTick()
{

}

AWeaponBase::EFireType AWeaponBase::GetPrimaryFireType()
{
	return AWeaponBase::EFireType::SIMPLE;
}

AWeaponBase::EFireType AWeaponBase::GetSecondaryFireType()
{
	return AWeaponBase::EFireType::SIMPLE;
}

FString AWeaponBase::GetNiceName()
{
	return "ERROR";
}

FVector AWeaponBase::GetGunOffset()
{
	return FVector::ZeroVector;
}

FString AWeaponBase::GetClass()
{
	return "weapon_null";
}

FString AWeaponBase::GetModel()
{
	return "null";
}

FString AWeaponBase::GetPrimaryAmmoType()
{
	return NULLAmmoType;
}

FString AWeaponBase::GetSecondaryAmmoType()
{
	return NULLAmmoType;
}

uint32 AWeaponBase::GetPrimaryClipSize()
{
	return 0;
}

uint32 AWeaponBase::GetSecondaryClipSize()
{
	return 0;
}

FString AWeaponBase::GetFireSoundName()
{
	return "null";
}

bool AWeaponBase::CanReloadClip(EClipType clip)
{
	return false;
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty, FDefaultAllocator> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeaponBase, ModelEquipped);
}

void AWeaponBase::RunFireSound_Implementation()
{
	if (AudioComp != NULL) {
		AudioComp->Play();
	}
}

//Weapons utility
void AWeaponBase::SetAttachLocation(EWeaponAttachPos newPos)
{
	switch (newPos)
	{
	case EWeaponAttachPos::LEFT_HAND:
		WorldModel->AttachTo(player->PlayerModel, "LHand_AttachPoint", EAttachLocation::SnapToTarget);
		break;
	case EWeaponAttachPos::RIGHT_HAND:
		WorldModel->AttachTo(player->PlayerModel, "RHand_AttachPoint", EAttachLocation::SnapToTarget);
		break;
	case EWeaponAttachPos::BODY_BACK:
		WorldModel->AttachTo(player->PlayerModel, "BBack_AttachPoint", EAttachLocation::SnapToTarget);
		break;
	case EWeaponAttachPos::BOTH_HANDS:
		//TODO: Implement this case
		break;
	}
}

bool AWeaponBase::ConsumeAmmo(EClipType clip, uint32 amount)
{
	switch (clip)
	{
	case EClipType::PRIMARY:
		if (CurPrimaryClipSize < amount) { return false; }
		CurPrimaryClipSize -= amount;
		break;
	case EClipType::SECONDARY:
		if (CurSecondaryClipSize < amount) { return false; }
		CurSecondaryClipSize -= amount;
		break;
	}
	return true;
}

bool AWeaponBase::ReloadClip(EClipType clip)
{
	if (!CanReloadClip(clip)) {
		return false;
	}

	switch (clip) {
	case EClipType::PRIMARY:
		if (player->GetRemainingAmmo(GetPrimaryAmmoType()) > 0) {
			if (player->GetRemainingAmmo(GetPrimaryAmmoType()) < GetPrimaryClipSize()) {
				uint32 toReload = player->GetRemainingAmmo(GetPrimaryAmmoType());
				player->RemoveAmmo(GetPrimaryAmmoType(), toReload);
				CurPrimaryClipSize = toReload;
			} else {
				CurPrimaryClipSize = GetPrimaryClipSize();
				player->RemoveAmmo(GetPrimaryAmmoType(), CurPrimaryClipSize);
			}
		} else {
			return false;
		}
		break;
	case EClipType::SECONDARY:
		if (player->GetRemainingAmmo(GetSecondaryAmmoType()) > 0) {
			if (player->GetRemainingAmmo(GetSecondaryAmmoType()) < GetSecondaryClipSize()) {
				uint32 toReload = player->GetRemainingAmmo(GetSecondaryAmmoType());
				player->RemoveAmmo(GetSecondaryAmmoType(), toReload);
				CurSecondaryClipSize = toReload;
			} else {
				CurSecondaryClipSize = GetSecondaryClipSize();
				player->RemoveAmmo(GetSecondaryAmmoType(), CurSecondaryClipSize);
			}
		} else {
			return false;
		}
		break;
	}
	return true;
}

void AWeaponBase::BroadcastFireSound()
{
	RunFireSound();
}

uint32 AWeaponBase::GetPrimaryAmmo()
{
	return CurPrimaryClipSize;
}

uint32 AWeaponBase::GetSecondaryAmmo()
{
	return CurSecondaryClipSize;
}
