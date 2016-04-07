// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "UModGameState.h"
#include "UModGameInstance.h"
#include "Renderer/Render3D.h"

void AUModGameState::HandleMatchIsWaitingToStart()
{
	UUModGameInstance* umod = Cast<UUModGameInstance>(GetGameInstance());

	if (!umod->IsDedicatedServer()) {
		URender3D::SetWorld(GetWorld());
	}

	FString MapPackageLongName = GetWorld()->GetCurrentLevel()->GetOutermost()->GetName();
	if (MapPackageLongName.Contains("/Game/Internal/")) {
		if (!umod->IsDedicatedServer()) {
			UUModGameInstance::ShowFatalMessage("You tried to load an Internal client map on the client with UModGameMode ! This is terribly bad issue, please tell Yuri6037 !");
			return;
		}
		UE_LOG(UMod_Maps, Error, TEXT("You tried to load an Internal client map ! This server will shut down to prevent future crash !"));
		FGenericPlatformMisc::RequestExit(true);
		return;
	}	
	
	bool hasStart = false;
	for (TObjectIterator<AActor> Itr; Itr; ++Itr) {
		FString cl = Itr->GetClass()->GetName();
		if (cl.Equals("PlayerStart", ESearchCase::Type::IgnoreCase)) { //The map has a player start
			hasStart = true;
		}
		if (cl.Equals("StaticMeshActor", ESearchCase::Type::IgnoreCase)) { //Check if is static if not remove
			TArray<UStaticMeshComponent*> comps;
			Itr->GetComponents<UStaticMeshComponent>(comps);

			bool del = false;
			for (int32 i = 0; i < comps.Num(); i++) {
				UStaticMeshComponent *c = comps[i];
				if (c->HasValidPhysicsState()) {
					del = true;
					break;
				}
			}

			if (del) {
				UE_LOG(UMod_Maps, Error, TEXT("StaticMeshActor with physics : this is not allowed, use SyncedPhysicsEntity instead ! Disabling physics."));
				Itr->DisableComponentsSimulatePhysics();				
			}
		}
	}

	if (!hasStart) {
		if (!umod->IsDedicatedServer()) {
			UUModGameInstance::ShowFatalMessage("The specified map has no start ! Please contact the map provider !");
			return;
		}
		UE_LOG(UMod_Maps, Error, TEXT("The specified map has no start ! Please contact the map provider !"));
		FGenericPlatformMisc::RequestExit(true);
	}
}



