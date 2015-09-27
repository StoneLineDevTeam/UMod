// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "SyncedMapPhysicsEntity.h"

#include "UnrealNetwork.h"


// Sets default values
ASyncedMapPhysicsEntity::ASyncedMapPhysicsEntity()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	EntityModel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EntityModel"));
	UStaticMesh *model = LoadObjFromPath<UStaticMesh>(TEXT("/Game/Models/RoundedCube"));
	EntityModel->SetStaticMesh(model);

	SetRootComponent(EntityModel);

	EntityModel->SetMobility(EComponentMobility::Movable);

	bReplicates = true;

	if (Role == ROLE_Authority) {
		EntityModel->SetSimulatePhysics(true);		
	} else {
		DisableComponentsSimulatePhysics();
		EntityModel->SetSimulatePhysics(false);
	}

	EntityModel->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

// Called when the game starts or when spawned
void ASyncedMapPhysicsEntity::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority) {
		EntityModel->WakeRigidBody();
	}
}

// Called every frame
void ASyncedMapPhysicsEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority){
		//Send physx simulation data		
		desiredPos = GetActorLocation();
		desiredRot = GetActorRotation();
		//PhysicsPacket(loc, rot);
	} else {
		//Interpolate between cur pos and new pos received from server
		FVector a = GetActorLocation();
		FVector b = desiredPos;
		FVector newPos = FMath::Lerp(a, b, 0.25F);
		SetActorLocation(newPos);

		FVector a1 = GetActorRotation().Vector();
		FVector b1 = desiredRot.Vector();
		FVector lerped = FMath::Lerp(a1, b1, 0.25F);		
		SetActorRotation(lerped.Rotation());
	}
}

FString ASyncedMapPhysicsEntity::GetName()
{
	return TEXT("SyncedMapPhysicsEntity");
}

/*void ASyncedMapPhysicsEntity::PhysicsPacket_Implementation(FVector newPos, FRotator newRot)
{
	if (Role == ROLE_Authority) {
		return;
	}

	desiredPos = newPos;
	desiredRot = newRot;
}*/

void ASyncedMapPhysicsEntity::GetLifetimeReplicatedProps(TArray<FLifetimeProperty, FDefaultAllocator> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASyncedMapPhysicsEntity, desiredPos);
	DOREPLIFETIME(ASyncedMapPhysicsEntity, desiredRot);
}

