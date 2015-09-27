// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UMod.h"
#include "UModCharacter.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"

#include "Game/UModGameMode.h"

//////////////////////////////////////////////////////////////////////////
// AUModCharacter

AUModCharacter::AUModCharacter(const FObjectInitializer& ObjectInitializer)	: Super(ObjectInitializer.DoNotCreateDefaultSubobject(ACharacter::MeshComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Create a CameraComponent	
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	PlayerCamera->AttachParent = GetCapsuleComponent();
	PlayerCamera->RelativeLocation = FVector(0, 0, 64.f);
	PlayerCamera->bUsePawnControlRotation = true;

	//Create a mesh component that will be used as world model
	PlayerModel = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerModel"));
	PlayerModel->SetOwnerNoSee(true);
	PlayerModel->AttachParent = PlayerCamera;
	PlayerModel->RelativeLocation = FVector(0.f, 0.f, -150.f);
	PlayerModel->bCastDynamicShadow = false;
	PlayerModel->CastShadow = false;

	if (Role == ROLE_Authority) {
		playerHealth = 100;
		playerMaxHealth = 100;
	}	
}

void AUModCharacter::BeginPlay()
{
	if (Role == ROLE_Authority) {
		AUModGameMode *gm = Cast<AUModGameMode>(GetWorld()->GetAuthGameMode());
		gm->OnPlayerInitialSpawn(this);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AUModCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Use", IE_Pressed, this, &AUModCharacter::HandleUse);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	InputComponent->BindAction("PrimaryFire", IE_Pressed, this, &AUModCharacter::StartPrimaryFire);
	InputComponent->BindAction("PrimaryFire", IE_Released, this, &AUModCharacter::EndPrimaryFire);
	InputComponent->BindAction("SecondaryFire", IE_Pressed, this, &AUModCharacter::StartSecondaryFire);
	InputComponent->BindAction("SecondaryFire", IE_Released, this, &AUModCharacter::EndSecondaryFire);
	InputComponent->BindAction("Reload", IE_Released, this, &AUModCharacter::HandleReload);

	InputComponent->BindAxis("MoveForward", this, &AUModCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AUModCharacter::MoveRight);
	
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void AUModCharacter::Tick(float DeltaSeconds)
{
	if (Role == ROLE_Authority && weapons[curWeapon] != NULL) {
		if (InFire1) {
			AWeaponBase *base = weapons[curWeapon];
			base->OnPlayerFire(0, AWeaponBase::EFireState::FIRING);
		}

		if (InFire2) {
			AWeaponBase *base = weapons[curWeapon];
			base->OnPlayerFire(1, AWeaponBase::EFireState::FIRING);
		}
	}
}

/* Input System */
void AUModCharacter::StartPrimaryFire()
{
	if (Role != ROLE_Authority) { 
		if (weapons[curWeapon] != NULL) {
			OnPlayerClick(0);
		}
		return;
	}

	if (weapons[curWeapon] != NULL) {
		AWeaponBase* w = weapons[curWeapon];
		if (w->GetPrimaryFireType() == AWeaponBase::EFireType::CONTINUES) {
			InFire1 = true;
			w->OnPlayerFire(0, AWeaponBase::EFireState::STARTED);
		} else {
			w->OnPlayerFire(0, AWeaponBase::EFireState::ENDED);
		}
	}
}
void AUModCharacter::EndPrimaryFire()
{
	if (Role != ROLE_Authority) {
		if (weapons[curWeapon] != NULL) {
			OnPlayerClick(0);
		}
		return;
	}

	if (weapons[curWeapon] != NULL) {
		AWeaponBase* w = weapons[curWeapon];
		if (w->GetPrimaryFireType() == AWeaponBase::EFireType::CONTINUES) {
			InFire1 = false;
			w->OnPlayerFire(0, AWeaponBase::EFireState::ENDED);
		} else {
			w->OnPlayerFire(0, AWeaponBase::EFireState::ENDED);
		}
	}
}
void AUModCharacter::StartSecondaryFire()
{
	if (Role != ROLE_Authority) {
		if (weapons[curWeapon] != NULL) {
			OnPlayerClick(1);
		}
		return;
	}

	if (weapons[curWeapon] != NULL) {
		AWeaponBase* w = weapons[curWeapon];
		if (w->GetSecondaryFireType() == AWeaponBase::EFireType::CONTINUES) {
			InFire2 = true;
			w->OnPlayerFire(1, AWeaponBase::EFireState::STARTED);
		} else {
			w->OnPlayerFire(1, AWeaponBase::EFireState::ENDED);
		}
	}
}
void AUModCharacter::EndSecondaryFire()
{
	if (Role != ROLE_Authority) {
		if (weapons[curWeapon] != NULL) {
			OnPlayerClick(1);
		}
		return;
	}

	if (weapons[curWeapon] != NULL) {
		AWeaponBase* w = weapons[curWeapon];
		if (w->GetSecondaryFireType() == AWeaponBase::EFireType::CONTINUES) {
			InFire2 = false;
			w->OnPlayerFire(1, AWeaponBase::EFireState::ENDED);
		} else {
			w->OnPlayerFire(1, AWeaponBase::EFireState::ENDED);
		}
	}
}
void AUModCharacter::HandleReload()
{
	if (Role != ROLE_Authority) {
		if (weapons[curWeapon] != NULL) {
			OnPlayerClick(2);
		}
		return;
	}

	if (weapons[curWeapon] != NULL) {
		AWeaponBase* w = weapons[curWeapon];
		w->OnPlayerFire(2, AWeaponBase::EFireState::ENDED);
	}
}
void AUModCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}
void AUModCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}
void AUModCharacter::HandleUse()
{
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;

	FHitResult result(ForceInit);
	FVector p = PlayerCamera->GetComponentLocation();
	FVector rot = PlayerCamera->GetComponentRotation().Vector();	
	bool hit = GetWorld()->LineTraceSingleByChannel(result, p, p + (rot * 2000), ECC_Pawn, RV_TraceParams);
		
	if (hit) {
		AActor *act = result.GetActor();
		FString str = act->GetClass()->GetName();
		if (str.Find(TEXT("_C"), ESearchCase::Type::CaseSensitive, ESearchDir::Type::FromStart, 0)) {
			FString realName = str.Replace(TEXT("_C"), TEXT(""), ESearchCase::Type::CaseSensitive);
			UE_LOG(UMod_Game, Warning, TEXT("BEntity : %s"), *realName);
		} else {
			UE_LOG(UMod_Game, Warning, TEXT("CEntity : %s"), *str);
		}
	}
}

void AUModCharacter::RemoveActiveWeapon()
{
	if (Role != ROLE_Authority) {
		UE_LOG(UMod_Game, Error, TEXT("Tried to remove weapon client side !"));
		return;
	}

	if (weapons[curWeapon] == NULL) {
		UE_LOG(UMod_Game, Warning, TEXT("Tried to access invalid pointer !"));
		return;
	}

	AWeaponBase* w = weapons[curWeapon];
	w->Destroy();
	weapons[curWeapon] = NULL;
}

void AUModCharacter::StripWeapons()
{
	if (Role != ROLE_Authority) {
		UE_LOG(UMod_Game, Error, TEXT("Tried to remove weapon client side !"));
		return;
	}

	for (int i = 0; i < 16; i++){
		if (weapons[i] != NULL) {
			AWeaponBase* w = weapons[i];
			w->Destroy();
			weapons[i] = NULL;
		}
	}		
}

AWeaponBase* AUModCharacter::GetActiveWeapon()
{
	return weapons[curWeapon];
}

AWeaponBase** AUModCharacter::GetWeapons()
{
	return weapons;
}

void AUModCharacter::GiveWeapon(UClass* cl)
{
	if (Role != ROLE_Authority){
		UE_LOG(UMod_Game, Error, TEXT("Tried to give weapon client side !"));
		return;
	}

	FVector plyPos = GetActorLocation();

	FVector pos = FVector::ZeroVector;//plyPos + GunOffset;
	
	UObject* obj = ANY_PACKAGE;
		
	if (cl == NULL || cl->GetSuperClass() != AWeaponBase::StaticClass()) {
		UE_LOG(UMod_Game, Error, TEXT("Unable to spawn weapon : weapon does not exist !"));
		return;
	}
	
	AWeaponBase* b = GetWorld()->SpawnActor<AWeaponBase>(cl, pos, FRotator::ZeroRotator);
	
	int slot = 0;
	for (int i = 0; i < 16; i++) {
		if (weapons[i] == NULL) {
			weapons[i] = b;
			slot = i;
			break;
		}
	}

	b->DoInit(this);

	if (slot == curWeapon){
		b->Equip();
	}
}

void AUModCharacter::SwitchWeapon(uint8 id)
{
	if (Role != ROLE_Authority) {
		UE_LOG(UMod_Game, Error, TEXT("Tried to switch weapon client side !"));
		return;
	}
	
	if (id == curWeapon){
		return;
	
	}
	AWeaponBase *b = weapons[curWeapon];
	b->UnEquip();

	curWeapon = id;

	AWeaponBase *b1 = weapons[curWeapon];
	b1->Equip();
}

void AUModCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty, FDefaultAllocator> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUModCharacter, weapons);
	DOREPLIFETIME(AUModCharacter, curWeapon);
	DOREPLIFETIME(AUModCharacter, playerHealth);
}

void AUModCharacter::UpdateClientSideData()
{

}

void AUModCharacter::OnPlayerClick_Implementation(uint8 but)
{
	switch (but) {
	case 0:
		if (weapons[curWeapon]->GetPrimaryFireType() == AWeaponBase::EFireType::CONTINUES) {
			InFire1 = !InFire1;
			if (InFire1) {
				weapons[curWeapon]->OnPlayerFire(but, AWeaponBase::EFireState::STARTED);
			} else {
				weapons[curWeapon]->OnPlayerFire(but, AWeaponBase::EFireState::ENDED);
			}
		} else {
			weapons[curWeapon]->OnPlayerFire(but, AWeaponBase::EFireState::ENDED);
		}
		break;
	case 1:
		if (weapons[curWeapon]->GetSecondaryFireType() == AWeaponBase::EFireType::CONTINUES) {
			InFire2 = !InFire2;
			if (InFire2) {
				weapons[curWeapon]->OnPlayerFire(but, AWeaponBase::EFireState::STARTED);
			} else {
				weapons[curWeapon]->OnPlayerFire(but, AWeaponBase::EFireState::ENDED);
			}
		} else {
			weapons[curWeapon]->OnPlayerFire(but, AWeaponBase::EFireState::ENDED);
		}
		break;
	case 2:
		weapons[curWeapon]->OnPlayerFire(but, AWeaponBase::EFireState::ENDED);
		break;
	}
}
bool AUModCharacter::OnPlayerClick_Validate(uint8 but)
{	
	if (weapons[curWeapon] == NULL) {
		return false;
	}
	return true;
}

void AUModCharacter::SetModel(FString path)
{
	if (Role != ROLE_Authority){
		return;
	}

	FString realPath = FString("/Game/Models/") + path;
	USkeletalMesh *m = LoadObjFromPath<USkeletalMesh>(*realPath);
	PlayerModel->SetSkeletalMesh(m);
}

void AUModCharacter::SetHealth(int32 var)
{
	if (Role != ROLE_Authority){
		return;
	}

	uint32 a = (uint32)var;

	if (a > playerMaxHealth){
		UE_LOG(UMod_Game, Error, TEXT("Tried to set health to more than max health : this is not allowed !"));
		return;
	}

	playerHealth = a;
}

void AUModCharacter::SetMaxHealth(uint32 var)
{
	if (Role != ROLE_Authority){
		return;
	}

	playerMaxHealth = var;
}

void AUModCharacter::DamagePlayer(int32 force)
{
	if (Role != ROLE_Authority){
		return;
	}

	uint32 a = (uint32)force;

	if (playerHealth > a){
		playerHealth -= a;
	} else if (playerHealth == a) {
		KillPlayer();
	} else if (playerHealth < a) {
		KillPlayer();
	}
}

void AUModCharacter::KillPlayer()
{
	if (Role != ROLE_Authority){
		return;
	}

	playerHealth = 0;
	AUModGameMode *gm = Cast<AUModGameMode>(GetWorld()->GetAuthGameMode());
	gm->OnPlayerDeath(this);
	
	PlayerModel->SetSimulatePhysics(true);	
}

uint32 AUModCharacter::GetHealth()
{
	return playerHealth;
}

uint32 AUModCharacter::GetMaxHealth()
{
	return playerMaxHealth;
}

void AUModCharacter::HealPlayer(int32 amount)
{
	if (Role != ROLE_Authority){
		return;
	}

	uint32 a = (uint32)amount;

	if (a > playerMaxHealth){
		UE_LOG(UMod_Game, Error, TEXT("Tried to set health to more than max health : this is not allowed !"));
		return;
	}
	playerHealth += a;
}

FVector AUModCharacter::GetEyeLocation()
{
	return PlayerCamera->GetComponentLocation();
}

FRotator AUModCharacter::GetEyeAngles()
{
	return PlayerCamera->GetComponentRotation();
}
