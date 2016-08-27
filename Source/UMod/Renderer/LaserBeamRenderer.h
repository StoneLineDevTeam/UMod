// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "LaserBeamRenderer.generated.h"

UCLASS()
class ALaserBeamRenderer : public AActor
{
	GENERATED_BODY()
	
public:	
	ALaserBeamRenderer();

	UFUNCTION()
	void Precache();
	
	virtual void BeginDestroy() override;
	
	void UpdateVectors(FVector Start, FVector End);

	UFUNCTION()
	void ClientOnVarsChanged();

	void ALaserBeamRenderer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty, FDefaultAllocator> & OutLifetimeProps) const;

	static ALaserBeamRenderer* CreateBeamRenderer(UWorld* world, FVector start, FVector end);

private:
	UPROPERTY(ReplicatedUsing = ClientOnVarsChanged)
		FVector Start;
	UPROPERTY(ReplicatedUsing = ClientOnVarsChanged)
		FVector End;
	UPROPERTY(ReplicatedUsing = ClientOnVarsChanged)
		USceneComponent* Attach;

	UParticleSystemComponent* comp;
	UParticleSystem* BeamEffect;
};
