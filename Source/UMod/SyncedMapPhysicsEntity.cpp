// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "SyncedMapPhysicsEntity.h"


// Sets default values
ASyncedMapPhysicsEntity::ASyncedMapPhysicsEntity()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASyncedMapPhysicsEntity::BeginPlay()
{
	Super::BeginPlay();
	
	if (Role != ROLE_Authority) {
		DisableComponentsSimulatePhysics();
	}
}

// Called every frame
void ASyncedMapPhysicsEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority){
		//Send physx simulation data
		UE_LOG(CoreLogger, Warning, TEXT("%s"), *GetActorLocation().ToString());
		FVector loc = GetActorLocation();
		FRotator rot = GetActorRotation();
		PhysicsPacket(loc, rot);
	} else {
		//Interpolate between cur pos and new pos received from server
		//FVector a = GetActorLocation();
		//FVector b = desiredPos;
		//FVector newPos = FMath::Lerp(a, b, 1.5F);
		SetActorLocation(desiredPos);

		//FVector a1 = GetActorRotation().Vector();
		//FVector b1 = desiredRot.Vector();
		//FVector lerped = FMath::Lerp(a1, b1, 1.5F);		
		SetActorRotation(desiredRot);
	}
}

void ASyncedMapPhysicsEntity::PhysicsPacket_Implementation(FVector newPos, FRotator newRot)
{
	if (Role == ROLE_Authority) {
		return;
	}

	desiredPos = newPos;
	desiredRot = newRot;
}

