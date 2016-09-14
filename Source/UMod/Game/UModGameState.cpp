// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "UModGameState.h"
#include "UModGameInstance.h"
#include "Renderer/Render3D.h"
#include "Entities/EntityBase.h"

void AUModGameState::HandleMatchIsWaitingToStart()
{
	if (Role == ROLE_Authority) {
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
				if (*Itr != NULL) {
					SpawnPos = Itr->GetActorLocation();
					SpawnRot = Itr->GetActorRotation();
					hasStart = true;
				}				
			}
			else if (cl.Equals("StaticMeshActor", ESearchCase::Type::IgnoreCase)) { //Check if is static if not remove
				TArray<UStaticMeshComponent*> comps;
				Itr->GetComponents<UStaticMeshComponent>(comps);

				bool del = false;
				for (int32 i = 0; i < comps.Num(); i++) {
					UStaticMeshComponent *c = comps[i];
					if (c->IsSimulatingPhysics()) {
						del = true;
						break;
					}
				}

				if (del) {
					UE_LOG(UMod_Maps, Error, TEXT("StaticMeshActor with physics : this is not allowed, use SyncedPhysicsEntity instead ! Disabling physics."));					
					FPlatformMisc::RequestExit(false); //EDIT 14/09/16 shut down the game in order to prevent physics desynchronization bug and potential Lua bugs/crashes ; use MapTools>CheckErrors in UMod Editor
					return;
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

	//Lua initialization system
	LuaInitShared();
	//End

	Super::HandleMatchIsWaitingToStart();
}

void AUModGameState::LuaInitShared()
{
	//Lua change : start implementing call to GM functions

	//Init Lua GameMode (only loads init.lua gamemode file currently)
	Game = Cast<UUModGameInstance>(GetGameInstance());
	FString LuaGameMode = Game->GetGameMode();
	if (Game == NULL) {
		UE_LOG(UMod_Game, Error, TEXT("UModGameInstance failed to retrieve in UModGameMode. Lua may be disfunctional..."));
		return;
	}
	if (Role == ROLE_Authority) {
		if (FPaths::FileExists(FPaths::GameDir() + "/GameModes/" + LuaGameMode + "/init.lua")) {
			Game->Lua->RunScript(FPaths::GameDir() + "/GameModes/" + LuaGameMode + "/init.lua");
		} else {
			UE_LOG(UMod_Lua, Warning, TEXT("Could not load %s : file does not exist. Aborting Lua GameMode load."), *FString("GameModes/" + LuaGameMode + "/init.lua"));
		}
	} else {
		FString file = UUModAssetsManager::Instance->GetLuaFile("GameModes/" + LuaGameMode + "/cl_init.lua");
		if (FPaths::FileExists(file)) {
			Game->Lua->RunScript(file);
		} else {
			UE_LOG(UMod_Lua, Warning, TEXT("Could not load %s : file does not exist. Aborting Lua GameMode load."), *FString("GameModes/" + LuaGameMode + "/cl_init.lua"));
		}
	}
	Game->Lua->InitGameMode();
}