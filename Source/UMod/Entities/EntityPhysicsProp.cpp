// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "EntityPhysicsProp.h"

AEntityPhysicsProp::AEntityPhysicsProp()
{
	/*FString InModel = "UMod:RoundedCube";
	GetInitProperty<FString>("Model", InModel);
	SetModel(InModel);*/
}

FString AEntityPhysicsProp::GetClass()
{
	return "PhysicsProp";
}

void AEntityPhysicsProp::OnInit()
{
	AddPhysicsObject();
	FString InModel = "UMod:RoundedCube";
	float InGravityScale = 1.0F;
	GetInitProperty<FString>("Model", InModel);	
	SetModel(InModel);
	SetCollisionModel(ECollisionType::COLLISION_PHYSICS);
	if (Role == ROLE_Authority) {
		GetInitProperty<float>("GravityScale", InGravityScale);
		PhysObj->SetGravityScale(InGravityScale);
	}
}

DEFINE_ENTITY(PhysicsProp, AEntityPhysicsProp::StaticClass())