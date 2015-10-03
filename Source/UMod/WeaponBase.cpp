// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "WeaponBase.h"

#include "Player/UModCharacter.h"


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

	bReplicates = 1;
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
		//ViewModel->SetOnlyOwnerSee(true);

		WorldModel->SetStaticMesh(wMesh);
		//WorldModel->SetOwnerNoSee(true);
	}

	player = ply;
	SetActorLocation(ply->GetActorLocation());
	AttachRootComponentToActor(ply);
			
	WorldModel->AttachTo(ply->PlayerModel, "RHand_AttachPoint", EAttachLocation::SnapToTarget);

	this->OnInit();
}

void AWeaponBase::Equip()
{
	//WorldModel->SetVisibility(true, true);
	//ViewModel->SetVisibility(true, true);
}

void AWeaponBase::UnEquip()
{
	//ViewModel->SetVisibility(false, true);
	//WorldModel->SetVisibility(false, true);
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
	return TEXT("ERROR");
}

FVector AWeaponBase::GetGunOffset()
{
	return FVector(100.0f, 30.0f, 10.0f);
}

FString AWeaponBase::GetClass()
{
	return TEXT("weapon_null");
}

FString AWeaponBase::GetModel()
{
	return TEXT("null");
}