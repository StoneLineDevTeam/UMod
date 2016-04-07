// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "EntityPhysicsProp.h"

AEntityPhysicsProp::AEntityPhysicsProp()
{
	FString InModel = "RoundedCube";
	GetInitProperty<FString>("Model", InModel);
	SetModel(InModel);

	SetPhysicsEnabled(true);
	SetCollisionModel(ECollisionType::COLLISION_PHYSICS);
}

FString AEntityPhysicsProp::GetClass()
{
	return "PhysicsProp";
}

void AEntityPhysicsProp::OnInit()
{
	FString InModel = "RoundedCube";
	float InGravityScale = 1.0F;
	GetInitProperty<FString>("Model", InModel);
	GetInitProperty<float>("GravityScale", InGravityScale);
	SetModel(InModel);
	SetGravityScale(InGravityScale);
	Construct();
}
