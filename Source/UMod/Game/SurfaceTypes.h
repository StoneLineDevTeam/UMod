#pragma once
#include "UMod.h"

static TMap<FString, struct FSurfaceType*> UModSurfaceTypes;

struct FSurfaceType {

	FString Identifier;
	FString FootSoundPath;
	FString HitSoundPath;
	FString DecalMatPath;
	FString ParticleMatPath;
		
	USoundWave *FootSound;
	USoundWave *HitSound;
	UMaterial *DecalMat;
	UMaterial *ParticleMat;

	bool IsValid;

	FSurfaceType(FString ID, FString FootSnd, FString HitSnd, FString DecalMat, FString ParticleMat);
	void Precache();

	void Register() {
		UModSurfaceTypes.Add(Identifier, this);
	}
};

namespace EUModSurfaceTypes {
	static FSurfaceType *Wood = new FSurfaceType("wood", "UMod:FootSteps/Wood", "UMod:Hits/Wood", "UMod:Decals/Wood", "UMod:FootParticles/Wood");
	static FSurfaceType *Glass = new FSurfaceType("glass", "UMod:FootSteps/Wood", "UMod:Hits/Wood", "UMod:Decals/Glass", "UMod:FootParticles/Glass");
	static FSurfaceType *Rock = new FSurfaceType("rock", "UMod:FootSteps/Wood", "UMod:Hits/Wood", "UMod:Decals/Rock", "UMod:FootParticles/Rock");
	static FSurfaceType *Metal = new FSurfaceType("metal", "UMod:FootSteps/Wood", "UMod:Hits/Wood", "UMod:Decals/Metal", "UMod:FootParticles/Metal");
	struct FRegisterSurfaceTypes {
		FRegisterSurfaceTypes() {
			Wood->Register();
			Glass->Register();
			Rock->Register();
			Metal->Register();
			//All registry goes here
		}
	};
}