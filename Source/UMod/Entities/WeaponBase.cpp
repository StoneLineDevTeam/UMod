// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "WeaponBase.h"
#include "UModGameInstance.h"
#include "Sound/SoundWave.h"

#include "Player/UModCharacter.h"

const FString AWeaponBase::NULLAmmoType = FString();

// Sets default values
AWeaponBase::AWeaponBase() : Super()
{
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
		AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));
	}
}

void AWeaponBase::ClientInit(AUModCharacter *ply)
{
	Game = Cast<UUModGameInstance>(GetGameInstance());

	if (!Initialized) {
		if (!GetWorldModel().Equals("null") && !GetViewModel().Equals("null")) {
			FString vMdlPath;
			FString wMdlPath;
			Game->AssetsManager->ResolveAsset(GetViewModel(), EUModAssetType::MODEL, vMdlPath);
			Game->AssetsManager->ResolveAsset(GetWorldModel(), EUModAssetType::MODEL, wMdlPath);
			UStaticMesh *vMesh = LoadObjFromPath<UStaticMesh>(*vMdlPath);
			UStaticMesh *wMesh = LoadObjFromPath<UStaticMesh>(*wMdlPath);

			ViewModel->SetStaticMesh(vMesh);
			ViewModel->SetOnlyOwnerSee(true);

			WorldModel->SetStaticMesh(wMesh);
			WorldModel->SetOwnerNoSee(true);
		}

		Player = ply;
		SetActorLocation(ply->GetRightHandLocation() + GetGunOffset());		
		AttachRootComponentToActor(ply);
		WorldModel->AttachToComponent(ply->PlayerModel, FAttachmentTransformRules::SnapToTargetIncludingScale, "RHand_AttachPoint");

		Initialized = true;
	}
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	Game = Cast<UUModGameInstance>(GetGameInstance());

	if (GetFireSoundName() != "null") {
		FString assetPath;
		EResolverResult res = Game->AssetsManager->ResolveAsset(GetFireSoundName(), EUModAssetType::SOUND, assetPath);
		if (res != EResolverResult::SUCCESS) {
			return;
		}
		FireSound = LoadObjFromPath<USoundWave>(*assetPath);		
		AudioComp->SetSound(FireSound);
	}
}	

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	this->OnTick();
}

void AWeaponBase::DoInit(AUModCharacter *ply)
{
	if (!GetWorldModel().Equals("null") && !GetViewModel().Equals("null")) {
		FString vMdlPath;
		FString wMdlPath;
		UUModAssetsManager::Instance->ResolveAsset(GetViewModel(), EUModAssetType::MODEL, vMdlPath);
		UUModAssetsManager::Instance->ResolveAsset(GetWorldModel(), EUModAssetType::MODEL, wMdlPath);
		UStaticMesh *vMesh = LoadObjFromPath<UStaticMesh>(*vMdlPath);
		UStaticMesh *wMesh = LoadObjFromPath<UStaticMesh>(*wMdlPath);

		ViewModel->SetStaticMesh(vMesh);
		ViewModel->SetOnlyOwnerSee(true);

		WorldModel->SetStaticMesh(wMesh);
		WorldModel->SetOwnerNoSee(true);
	}

	Player = ply;
	SetActorLocation(ply->GetRightHandLocation() + GetGunOffset());
	AttachRootComponentToActor(ply);

	WorldModel->AttachToComponent(ply->PlayerModel, FAttachmentTransformRules::SnapToTargetIncludingScale, "RHand_AttachPoint");

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
	if (Player == NULL) {
		return;
	}

	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.AddIgnoredActor(Player);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;

	FHitResult result(ForceInit);
	FVector p = Player->GetEyeLocation();
	FVector rot = Player->GetEyeAngles().Vector();
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

FString AWeaponBase::GetWorldModel()
{
	return "null";
}

FString AWeaponBase::GetViewModel()
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
		WorldModel->AttachToComponent(Player->PlayerModel, FAttachmentTransformRules::SnapToTargetIncludingScale, "LHand_AttachPoint");
		break;
	case EWeaponAttachPos::RIGHT_HAND:
		WorldModel->AttachToComponent(Player->PlayerModel, FAttachmentTransformRules::SnapToTargetIncludingScale, "RHand_AttachPoint");
		break;
	case EWeaponAttachPos::BODY_BACK:
		WorldModel->AttachToComponent(Player->PlayerModel, FAttachmentTransformRules::SnapToTargetIncludingScale, "BBack_AttachPoint");
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
		if (Player->GetRemainingAmmo(GetPrimaryAmmoType()) > 0) {
			if (Player->GetRemainingAmmo(GetPrimaryAmmoType()) < GetPrimaryClipSize()) {
				uint32 toReload = Player->GetRemainingAmmo(GetPrimaryAmmoType());
				Player->RemoveAmmo(GetPrimaryAmmoType(), toReload);
				CurPrimaryClipSize = toReload;
			} else {
				CurPrimaryClipSize = GetPrimaryClipSize();
				Player->RemoveAmmo(GetPrimaryAmmoType(), CurPrimaryClipSize);
			}
		} else {
			return false;
		}
		break;
	case EClipType::SECONDARY:
		if (Player->GetRemainingAmmo(GetSecondaryAmmoType()) > 0) {
			if (Player->GetRemainingAmmo(GetSecondaryAmmoType()) < GetSecondaryClipSize()) {
				uint32 toReload = Player->GetRemainingAmmo(GetSecondaryAmmoType());
				Player->RemoveAmmo(GetSecondaryAmmoType(), toReload);
				CurSecondaryClipSize = toReload;
			} else {
				CurSecondaryClipSize = GetSecondaryClipSize();
				Player->RemoveAmmo(GetSecondaryAmmoType(), CurSecondaryClipSize);
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
