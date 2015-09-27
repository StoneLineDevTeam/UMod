// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "UModGameState.h"

void AUModGameState::HandleMatchIsWaitingToStart()
{
	for (TObjectIterator<AActor> Itr; Itr; ++Itr) {
		FString cl = Itr->GetClass()->GetName();
		if (cl.Equals(TEXT("StaticMeshActor"), ESearchCase::Type::IgnoreCase)) { //Check if is static if not remove
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
				return;
			}
		}
	}
}



