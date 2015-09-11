// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UMod.h"
#include "UModCharacter.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"

#include "UnrealNetwork.h"

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

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	//Create a mesh component that will be used as world model
	PlayerModel = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PlayerModel"));
	PlayerModel->SetOwnerNoSee(true);
	PlayerModel->AttachParent = PlayerCamera;
	PlayerModel->RelativeLocation = FVector(0.f, 0.f, -150.f);
	PlayerModel->bCastDynamicShadow = false;
	PlayerModel->CastShadow = false;

	//GiveWeapon(TEXT("WeaponTest"));
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
	
	InputComponent->BindAction("Fire", IE_Pressed, this, &AUModCharacter::OnFire);
	
	InputComponent->BindAxis("MoveForward", this, &AUModCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AUModCharacter::MoveRight);
	
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

/* Input System */
void AUModCharacter::OnFire()
{ 
	if (Role != ROLE_Authority) { 
		if (weapons[curWeapon] != NULL) {
			OnPlayerClick(0);
		}
		return;
	}

	if (weapons[curWeapon] != NULL) {
		AWeaponBase* w = weapons[curWeapon];
		w->OnPlayerFire(0);
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
	/*if (Role != ROLE_Authority) {
		UE_LOG(CoreLogger, Error, TEXT("Tried to use something client side !"));
		return;
	}*/

	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bTraceAsyncScene = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;

	FHitResult result(ForceInit);
	FVector p = PlayerCamera->GetComponentLocation();
	FVector rot = PlayerCamera->GetComponentRotation().Vector();	
	bool hit = GetWorld()->LineTraceSingleByChannel(
		result,        //result
		p,    //start
		p + (rot * 100), //end
		ECC_Pawn, //collision channel
		RV_TraceParams
	);
		
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

void AUModCharacter::GiveWeapon(FString base)
{
	if (Role != ROLE_Authority){
		UE_LOG(UMod_Game, Error, TEXT("Tried to give weapon client side !"));
		return;
	}

	FVector plyPos = GetActorLocation();

	FVector pos = plyPos + GunOffset;
	
	UObject* obj = ANY_PACKAGE;

	FString src = FString("/Game/UMod/Weapons/");
	
	UClass* cl = FindObject<UClass>(obj, *base, true);

	if (cl == NULL) {
		UE_LOG(UMod_Game, Error, TEXT("Unable to spawn weapon : FindObject returned an Invalid Pointer !"));
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
		this->UpdateAttachement();
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
	curWeapon = id;
	this->UpdateAttachement();
}

void AUModCharacter::UpdateAttachement()
{
	//TODO : Make attachement system
}

void AUModCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty, FDefaultAllocator> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AUModCharacter, weapons);
	DOREPLIFETIME(AUModCharacter, curWeapon);
}

void AUModCharacter::UpdateClientSideData()
{

}

void AUModCharacter::OnPlayerClick_Implementation(uint8 but)
{
	weapons[curWeapon]->OnPlayerFire(but);
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
	FString realPath = FString("/Game/Models/") + path;
	USkeletalMesh *m = LoadObjFromPath<USkeletalMesh>(*realPath);
	PlayerModel->SetSkeletalMesh(m);
}