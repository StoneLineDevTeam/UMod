// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "LaserBeamRenderer.h"
#include "UModAssetsManager.h"


ALaserBeamRenderer::ALaserBeamRenderer()
{
	bReplicates = true;
	BeamEffect = LoadObjFromPath<UParticleSystem>("/Game/UMod/Particles/LaserBeam");
	comp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleEmitter"));
	comp->bAutoDestroy = true;
	if (BeamEffect != NULL) {
		comp->SetTemplate(BeamEffect);
	}
	RootComponent = comp;

	UUModAssetsManager::PrecacheAssets.AddDynamic(this, &ALaserBeamRenderer::Precache);
}

void ALaserBeamRenderer::Precache()
{
	FString realPath;
	EResolverResult res = UUModAssetsManager::Instance->ResolveAsset("UMod:Particles/LaserBeam", EUModAssetType::OTHER, realPath);
	if (res != EResolverResult::SUCCESS) {
		UE_LOG(UMod_Game, Error, TEXT("Unable to load LaserBeam particle system"));
		return;
	}
	BeamEffect = LoadObjFromPath<UParticleSystem>(*realPath);
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

