// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UMod.h"
#include "UModCharacter.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"
#include "Game/UModGameMode.h"
#include "UModController.h"

//////////////////////////////////////////////////////////////////////////
// AUModCharacter

AUModCharacter::AUModCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer.DoNotCreateDefaultSubobject(ACharacter::MeshComponentName))
{
	// Set size for collision capsule
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
	bAlwaysRelevant = true;
	bReplicates = true;
	bReplicateMovement = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	//Create a mesh component that will be used as world model
	PlayerModel = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerModel"));
	PlayerModel->SetOwnerNoSee(false);
	PlayerModel->SetupAttachment(GetCapsuleComponent());
	PlayerModel->RelativeLocation = FVector(0.f, 0.f, -150.f);
	PlayerModel->bCastDynamicShadow = false;
	PlayerModel->CastShadow = false;

	CameraHack = CreateDefaultSubobject<USceneComponent>(TEXT("CameraHack"));
	CameraHack->SetupAttachment(PlayerModel);
	CameraHack->RelativeLocation = FVector(0, 0, 64.f);

	// Create a CameraComponent	
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));	
	PlayerCamera->bUsePawnControlRotation = true;
	PlayerCamera->SetupAttachment(CameraHack);

	SpotLight = CreateDefaultSubobject<USpotLightComponent>("FlashLight");
	SpotLight->RelativeLocation = FVector(0, 0, 50);
	SpotLight->SetupAttachment(PlayerCamera);

	if (Role == ROLE_Authority) {
		playerHealth = 100;
		playerMaxHealth = 100;
	}
}

void AUModCharacter::BeginPlay()
{
	Super::BeginPlay();

	AUModController *ctrl = Cast<AUModController>(GetController());
	if (ctrl != NULL) {
		ctrl->NativePlayer = this;
	}

	if (Role == ROLE_Authority) {
		AUModGameMode *gm = Cast<AUModGameMode>(GetWorld()->GetAuthGameMode());
		gm->OnPlayerSpawn(this);
	} else {
		//ctrl->PlayerCameraManager->CameraCache.POV.Location = FVector(-5, -5, -5);
		//Little post process test (MotionBlur) : DON'T EVER TRY 100.9F and 200.9F Screen Gets COMPLETELY ENTIRELY distorded
		//PlayerCamera->PostProcessSettings.bOverride_MotionBlurAmount = true;
		//PlayerCamera->PostProcessSettings.bOverride_MotionBlurMax = true;
		//PlayerCamera->PostProcessSettings.MotionBlurAmount = 100.9F;
		//PlayerCamera->PostProcessSettings.MotionBlurMax = 200.9F;
		//PlayerCamera->PostProcessSettings.bOverride_AutoExposureMaxBrightness = true;
		//PlayerCamera->PostProcessSettings.bOverride_AutoExposureMinBrightness = true;
		//PlayerCamera->PostProcessSettings.AutoExposureMaxBrightness = 9.0F;
		//PlayerCamera->PostProcessSettings.AutoExposureMinBrightness = -9.0F;
		//PlayerCamera->PostProcessSettings.bOverride_SceneColorTint = true;
		//PlayerCamera->PostProcessSettings.SceneColorTint = FLinearColor(FColor(255, 192, 203));
	}
}

void AUModCharacter::EndPlay(EEndPlayReason::Type reason)
{
	if (Role == ROLE_Authority) {
		AUModGameMode *GameMode = Cast<AUModGameMode>(GetWorld()->GetAuthGameMode());
		GameMode->OnPlayerDeath(this);
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AUModCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	//Special actions
	InputComponent->BindAction("Use", IE_Pressed, this, &AUModCharacter::HandleUseStart);
	InputComponent->BindAction("Use", IE_Released, this, &AUModCharacter::HandleUseEnd);
	InputComponent->BindAction("FlashLight", IE_Pressed, this, &AUModCharacter::HandleFlashLight);
	InputComponent->BindAction("WeaponPrev", IE_Pressed, this, &AUModCharacter::HandleWeaponPrev);
	InputComponent->BindAction("WeaponNext", IE_Pressed, this, &AUModCharacter::HandleWeaponNext);

	//Jump
	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	//Weapon system
	InputComponent->BindAction("PrimaryFire", IE_Pressed, this, &AUModCharacter::StartPrimaryFire);
	InputComponent->BindAction("PrimaryFire", IE_Released, this, &AUModCharacter::EndPrimaryFire);
	InputComponent->BindAction("SecondaryFire", IE_Pressed, this, &AUModCharacter::StartSecondaryFire);
	InputComponent->BindAction("SecondaryFire", IE_Released, this, &AUModCharacter::EndSecondaryFire);
	InputComponent->BindAction("Reload", IE_Released, this, &AUModCharacter::HandleReload);

	//Movement
	InputComponent->BindAxis("MoveForward", this, &AUModCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AUModCharacter::MoveRight);
	//Mouse
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void AUModCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
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
void AUModCharacter::HandleUseStart()
{	
	if (Role != ROLE_Authority) {
		OnPlayerSpecialKey(0, true);
	} else {
		OnPlayerSpecialKey_Implementation(0, true);
	}
}
void AUModCharacter::HandleUseEnd()
{	
	if (Role != ROLE_Authority) {
		OnPlayerSpecialKey(0, false);
	}
	else {
		OnPlayerSpecialKey_Implementation(0, false);
	}
}
void AUModCharacter::HandleFlashLight()
{
	if (Role != ROLE_Authority) {
		OnPlayerSpecialKey(1, false);
	} else {
		OnPlayerSpecialKey_Implementation(1, false);
	}
}
void AUModCharacter::HandleWeaponPrev()
{
	if (Role != ROLE_Authority) {
		UE_LOG(UMod_Input, Log, TEXT("[DEBUG]OnPlayerSpecialKey->PrevWeapon"));
		OnPlayerSpecialKey(2, false);
	} else {
		OnPlayerSpecialKey_Implementation(2, false);
	}
}
void AUModCharacter::HandleWeaponNext()
{
	if (Role != ROLE_Authority) {
		UE_LOG(UMod_Input, Log, TEXT("[DEBUG]OnPlayerSpecialKey->NextWeapon"));
		OnPlayerSpecialKey(3, false);
	} else {
		OnPlayerSpecialKey_Implementation(3, false);
	}
}
/* End */


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
	DOREPLIFETIME(AUModCharacter, PlayerAmmo);	

	AUTO_NWVARS_REP_CODE(AUModCharacter);
}

void AUModCharacter::UpdateClientSideData()
{
	AWeaponBase *b = weapons[curWeapon];
	if (b != NULL) {
		b->ClientInit(this);
	}
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
	return weapons[curWeapon] != NULL;
}
void AUModCharacter::OnPlayerSpecialKey_Implementation(uint8 bind, bool pressed)
{
	if (bind == 0) { //IN_USE
		if (pressed) {
			UseKey = true;
		} else {
			UseKey = false;
		}
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
			}
			else {
				UE_LOG(UMod_Game, Warning, TEXT("CEntity : %s"), *str);
			}
		}
	} else if (bind == 1) { //IN_LIGHT	
		if (SpotLight != NULL) {
			UE_LOG(UMod_Game, Warning, TEXT("You used your spotlight."));
			if (SpotLight->IsActive()) {
				SpotLight->Deactivate();
			} else {
				SpotLight->Activate();
			}
		}
	} else if (bind == 2) { //IN_WEAPON_PREV
		UE_LOG(UMod_Input, Log, TEXT("[DEBUG]OnPlayerSpecialKey->PrevWeapon"));
		AWeaponBase *base = weapons[curWeapon];
		if (base != NULL) {
			base->OnWeaponPrev();
		}
	} else if (bind == 3) { //IN_WEAPON_NEXT
		UE_LOG(UMod_Input, Log, TEXT("[DEBUG]OnPlayerSpecialKey->NextWeapon"));
		AWeaponBase *base = weapons[curWeapon];
		if (base != NULL) {
			base->OnWeaponNext();
		}
	}
}
bool AUModCharacter::OnPlayerSpecialKey_Validate(uint8 bind, bool pressed)
{
	return SpotLight != NULL;
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
	Destroy();
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
	return Controller->GetControlRotation();	
}

void AUModCharacter::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult)
{
	OutResult.Location = GetEyeLocation();
	OutResult.Rotation = GetEyeAngles();
}

FVector AUModCharacter::GetRightHandLocation()
{
	FTransform transform = PlayerModel->GetSocketTransform("RHand_AttachPoint");
	return transform.GetLocation();
}

void AUModCharacter::GiveAmmo(FString ammoType, uint32 amount)
{
	if (Role != ROLE_Authority) {
		return;
	}

}

void AUModCharacter::RemoveAmmo(FString ammoType, uint32 amount)
{
	if (Role != ROLE_Authority) {
		return;
	}
}

uint32 AUModCharacter::GetRemainingAmmo(FString ammoType)
{
	return 0;
}

FVector AUModCharacter::GetLeftHandLocation()
{
	FTransform transform = PlayerModel->GetSocketTransform("LHand_AttachPoint");
	return transform.GetLocation();
}

bool AUModCharacter::IsUseKeyDown()
{
	return UseKey;
}

void AUModCharacter::AddPhysicsObject() //UModCharacter doesn't use regular physics
{
	UE_LOG(UMod_Game, Error, TEXT("Attempted to use physics on UModCharacter !"));
}

FPhysObj *AUModCharacter::GetPhysicsObject()
{
	return NULL;
}

void AUModCharacter::SetModel(FString path)
{

}

FString AUModCharacter::GetModel()
{
	return "";
}

void AUModCharacter::SetCollisionModel(ECollisionType collision) //UModCharacter doesn't use regular physics
{
	UE_LOG(UMod_Game, Error, TEXT("Attempted to use physics on UModCharacter !"));
}

ECollisionType AUModCharacter::GetCollisionModel()
{
	return ECollisionType::COLLISION_PHYSICS;
}

void AUModCharacter::SetMaterial(FString path)
{
	//Not supported right now
}

void AUModCharacter::SetSubMaterial(int32 index, FString path)
{
	//Not supported right now
}

FString AUModCharacter::GetMaterial()
{
	return "";
}

FString AUModCharacter::GetSubMaterial(int32 index)
{	
	return "";
}

int32 AUModCharacter::GetSubMaterialsNum()
{
	return 0;
}

int AUModCharacter::GetLuaRef()
{
	return LuaReference;
}

void AUModCharacter::SetLuaRef(int r)
{
	LuaReference = r;
}

void AUModCharacter::SetLuaClass(FString s)
{
	LuaClassName = s;
}

void AUModCharacter::LuaUnRef()
{
	UUModGameInstance *Game = Cast<UUModGameInstance>(GetGameInstance());
	Game->Lua->Lua->UnRef(LuaReference);
	LuaReference = LUA_NOREF;
}

FString AUModCharacter::GetClass()
{
	if (!LuaClassName.IsEmpty()) {
		return "LuaPlayer_" + LuaClassName;
	}
	return "Player";
}

void AUModCharacter::Remove()
{
	KillPlayer();
}

void AUModCharacter::SetPos(FVector vec)
{
	SetActorLocation(vec);
}

void AUModCharacter::SetAngles(FRotator ang)
{
	SetActorRotation(ang);
}

FVector AUModCharacter::GetPos()
{
	return GetActorLocation();
}

FRotator AUModCharacter::GetAngles()
{
	return GetActorRotation();
}

void AUModCharacter::SetColor(FColor col)
{
	//I need my material utilities for that
}

FColor AUModCharacter::GetColor()
{
	return FColor(0, 0, 0); //I need my material utilities for that
}

AUTO_NWVARS_BODY(AUModCharacter)

EWaterLevel AUModCharacter::GetWaterLevel()
{
	return EWaterLevel::NULL_SUMBERGED;
}

int AUModCharacter::EntIndex()
{
	return GetUniqueID();
}

DEFINE_ENTITY(Player, AUModCharacter)