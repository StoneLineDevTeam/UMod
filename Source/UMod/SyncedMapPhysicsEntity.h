// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SyncedMapPhysicsEntity.generated.h"

UCLASS()
class UMOD_API ASyncedMapPhysicsEntity : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASyncedMapPhysicsEntity();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	FVector desiredPos;
	FRotator desiredRot;

	UFUNCTION(NetMulticast, UnReliable)
	void PhysicsPacket(FVector newPos, FRotator newRot);
	void PhysicsPacket_Implementation(FVector newPos, FRotator newRot);
};
