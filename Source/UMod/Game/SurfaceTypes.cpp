#include "UMod.h"
#include "SurfaceTypes.h"
#include "UModAssetsManager.h"

void FSurfaceType::Precache()
{
	IsValid = false;	

	UUModAssetsManager *AssetsManager = UUModAssetsManager::Instance;
	FString RealPath;
	
	EResolverResult res = AssetsManager->ResolveAsset(FootSoundPath, EUModAssetType::SOUND, RealPath);
	if (res != EResolverResult::SUCCESS) {
		UE_LOG(UMod_Game, Error, TEXT("Failed to precache FSurfaceType %s, %s"), *Identifier, *AssetsManager->GetErrorMessage(res));		
		return;
	}
	
	FootSound = LoadObjFromPath<USoundWave>(*RealPath);
	
	res = AssetsManager->ResolveAsset(HitSoundPath, EUModAssetType::SOUND, RealPath);
	if (res != EResolverResult::SUCCESS) {
		UE_LOG(UMod_Game, Error, TEXT("Failed to precache FSurfaceType %s, %s"), *Identifier, *AssetsManager->GetErrorMessage(res));		
		return;
	}

	HitSound = LoadObjFromPath<USoundWave>(*RealPath);
	
	res = AssetsManager->ResolveAsset(DecalMatPath, EUModAssetType::MATERIAL, RealPath);
	if (res != EResolverResult::SUCCESS) {
		UE_LOG(UMod_Game, Error, TEXT("Failed to precache FSurfaceType %s, %s"), *Identifier, *AssetsManager->GetErrorMessage(res));		
		return;
	}

	DecalMat = LoadObjFromPath<UMaterial>(*RealPath);
	
	res = AssetsManager->ResolveAsset(ParticleMatPath, EUModAssetType::MATERIAL, RealPath);
	if (res != EResolverResult::SUCCESS) {
		UE_LOG(UMod_Game, Error, TEXT("Failed to precache FSurfaceType %s, %s"), *Identifier, *AssetsManager->GetErrorMessage(res));		
		return;
	}

	ParticleMat = LoadObjFromPath<UMaterial>(*RealPath);
	
	//Only adding to root if all assets have been correctly loaded
	ParticleMat->AddToRoot();
	DecalMat->AddToRoot();
	HitSound->AddToRoot();
	FootSound->AddToRoot();
	IsValid = true;
}

FSurfaceType::FSurfaceType(FString ID, FString FootSnd, FString HitSnd, FString DecalMt, FString ParticleMt)
{
	Identifier = ID;
	FootSoundPath = FootSnd;
	HitSoundPath = HitSnd;
	DecalMatPath = DecalMt;
	ParticleMatPath = ParticleMt;	
}
