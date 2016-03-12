// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "LaserBeamRenderer.h"


ALaserBeamRenderer::ALaserBeamRenderer()
{
	bReplicates = true;
	BeamEffect = LoadObjFromPath<UParticleSystem>("/Game/Particles/LaserBeam");
	comp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleEmitter"));
	comp->bAutoDestroy = true;
	comp->SetTemplate(BeamEffect);
	RootComponent = comp;
}

void ALaserBeamRenderer::BeginDestroy()
{
	Super::BeginDestroy();
}

void ALaserBeamRenderer::UpdateVectors(FVector s, FVector e)
{
	Start = s;
	End = e;
	if (Role == ROLE_Authority) {
		comp->SetVectorParameter("Source", Start);
		comp->SetVectorParameter("Target", End);
	}
}

ALaserBeamRenderer* ALaserBeamRenderer::CreateBeamRenderer(UWorld *world, FVector start, FVector end)
{
	ALaserBeamRenderer* render = world->SpawnActor<ALaserBeamRenderer>(ALaserBeamRenderer::StaticClass(), start, FRotator::ZeroRotator);
	render->UpdateVectors(start, end);
	return render;
}

void ALaserBeamRenderer::ClientOnVarsChanged()
{
	comp->SetVectorParameter("Source", Start);
	comp->SetVectorParameter("Target", End);
}

void ALaserBeamRenderer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty, FDefaultAllocator> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALaserBeamRenderer, Start);
	DOREPLIFETIME(ALaserBeamRenderer, End);
	DOREPLIFETIME(ALaserBeamRenderer, Attach);
}

