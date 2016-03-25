// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "EntityPhysicsProp.h"

AEntityPhysicsProp::AEntityPhysicsProp()
{
	/*FString *ptr = GetInitProperty("Model");
	if (ptr != NULL) {
		SetModel(*ptr);
	} else {
		SetModel("RoundedCube");
	}*/
	/*FString InModel = "RoundedCube";
	float InGravityScale = 1.0F;
	GetInitProperty<FString>("Model", InModel);
	GetInitProperty<float>("GravityScale", InGravityScale);
	SetModel(InModel);
	SetGravityScale(InGravityScale);*/
	SetModel("RoundedCube");
	SetPhysicsEnabled(true);
	SetCollisionEnabled(true);
	Construct();
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
}
