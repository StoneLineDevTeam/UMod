// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UMod.h"
#include "UModCharacter.h"
#include "UModProjectile.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"

//////////////////////////////////////////////////////////////////////////
// AUModCharacter

AUModCharacter::AUModCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->RelativeLocation = FVector(0.f, 0.f, -150.f);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AUModCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindings
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	InputComponent->BindAction("Fire", IE_Pressed, this, &AUModCharacter::OnFire);
	
	InputComponent->BindAxis("MoveForward", this, &AUModCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AUModCharacter::MoveRight);
	
	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AUModCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AUModCharacter::LookUpAtRate);
}

void AUModCharacter::OnFire()
{ 
	if (Role != ROLE_Authority) { 
		UE_LOG(CoreLogger, Error, TEXT("Tried to fire weapon client side !"));		
		return;
	}

	if (weapons[curWeapon] != NULL) {
		AWeaponBase* w = weapons[curWeapon];
		w->OnPlayerFire(0);
	}

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

void AUModCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AUModCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AUModCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AUModCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AUModCharacter::RemoveActiveWeapon()
{
	if (Role != ROLE_Authority) {
		UE_LOG(CoreLogger, Error, TEXT("Tried to remove weapon client side !"));
		return;
	}

	if (weapons[curWeapon] == NULL) {
		UE_LOG(CoreLogger, Warning, TEXT("Tried to access invalid pointer !"));
		return;
	}

	AWeaponBase* w = weapons[curWeapon];
	w->Destroy();
	weapons[curWeapon] = NULL;
}

void AUModCharacter::StripWeapons()
{
	if (Role != ROLE_Authority) {
		UE_LOG(CoreLogger, Error, TEXT("Tried to remove weapon client side !"));
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
		UE_LOG(CoreLogger, Error, TEXT("Tried to give weapon client side !"));
		return;
	}

	FVector plyPos = GetActorLocation();

	FVector pos = plyPos + GunOffset;
	
	UObject* obj = ANY_PACKAGE;

	UClass* cl = FindObject<UClass>(obj, *base, true);
	
	AWeaponBase* b = GetWorld()->SpawnActor<AWeaponBase>(cl, pos, FRotator(0, 0, 0).ZeroRotator);

	for (int i = 0; i < 16; i++) {
		if (weapons[i] == NULL) {
			weapons[i] = b;
			break;
		}
	}
}

void AUModCharacter::SwitchWeapon(uint8 id)
{
	if (Role != ROLE_Authority) {
		UE_LOG(CoreLogger, Error, TEXT("Tried to switch weapon client side !"));
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

void AUModCharacter::OnSwitchChanged()
{
	//TODO : Make data updating system using APlayerState
}