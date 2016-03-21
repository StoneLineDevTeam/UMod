// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Entities/EntityBase.h"
#include "EntityPhysicsProp.generated.h"

/**
 * 
 */
UCLASS()
class UMOD_API AEntityPhysicsProp : public AEntityBase
{
	GENERATED_BODY()

	AEntityPhysicsProp();
public:
	//Editor only in wait of fix for startup property map edit PB
	UPROPERTY(EditAnywhere)
	float GravityScale = 1;
	UPROPERTY(EditAnywhere)
	FString Model = "RoundedCube";

	virtual FString GetClass();
	virtual void OnInit();
};
