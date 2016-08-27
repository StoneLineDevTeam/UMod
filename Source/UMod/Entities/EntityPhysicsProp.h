// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Entities/EntityBase.h"
#include "EntityPhysicsProp.generated.h"

UCLASS()
class AEntityPhysicsProp : public AEntityBase
{
	GENERATED_BODY()

	AEntityPhysicsProp();
public:
	virtual FString GetClass();
	virtual void OnInit();
};