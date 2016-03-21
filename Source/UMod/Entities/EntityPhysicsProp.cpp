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
	SetModel(Model);
	SetGravityScale(GravityScale);
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
	SetModel(Model);
	SetGravityScale(GravityScale);
}
