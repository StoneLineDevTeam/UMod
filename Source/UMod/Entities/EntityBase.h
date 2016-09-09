// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Entity.h"
#include "EntityBase.generated.h"

USTRUCT()
struct FInitProperty {
	GENERATED_USTRUCT_BODY()

	FInitProperty(FString n, FString v) {
		Name = n;
		Value = v;
	}

	FInitProperty() {
	}

	UPROPERTY(EditAnywhere)
	FString Name;
	UPROPERTY(EditAnywhere)
	FString Value;

	bool operator==(FString other){
		return other == Name;
	}
};

class UUModGameInstance;

UCLASS()
class AEntityBase : public AActor, public Entity
{
	GENERATED_BODY()
	
	class UStaticMeshComponent *EntityModel;
private:
	//Network stuff
	UPROPERTY(Replicated)
	FVector DesiredPos;
	UPROPERTY(Replicated)
	FRotator DesiredRot;
	UPROPERTY(ReplicatedUsing = UpdateCollisionStatus)
	uint8 CurCollisionProfile;
	UPROPERTY(ReplicatedUsing = UpdateClientMDL)
	FString ServerMDLSync;
	//UPROPERTY(ReplicatedUsing = UpdateClientMAT)
	FString* ServerMATSync;

	UFUNCTION()
	void UpdateClientMDL();
	UFUNCTION()
	void UpdateCollisionStatus();
	UFUNCTION()
	void UpdateClientMAT();
	//End

	/* Tried to do this in macros however it seam that UBT runs UHT before C++ preprocessor... */
	UPROPERTY(Replicated)
	TMap<FString, int> NWInts;
	UPROPERTY(Replicated)
	TMap<FString, FString> NWStrings;
	UPROPERTY(Replicated)
	TMap<FString, uint32> NWUInts;
	UPROPERTY(Replicated)
	TMap<FString, float> NWFloats;

	//Does this entity manages physics
	//bool PhysEnabled = false; //Removal 17/08/2014 : Physics implementation restructuration for easier usage (all physics methods are inside FPhysObj now

	bool Initializing = false;

	FString CurMdl;
#if WITH_EDITOR
	//FIX : Editor not updating model
	FString EditorCurMdl;
#endif
	
	UPROPERTY(EditAnywhere)
	TArray<FInitProperty> InitProperties;

	int LuaReference; //Yes starting LuaEntityBase !
	FString LuaClassName; //The lua class name
protected:
	UUModGameInstance *Game;

	FPhysObj *PhysObj;
public:	
	AEntityBase();

	/*Begin AActor interface*/
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;	
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty, FDefaultAllocator> & OutLifetimeProps) const;
	UFUNCTION()
	void ActorBeginOverlap(UPrimitiveComponent* comp, AActor* OtherActor, class UPrimitiveComponent *C, int32 i, bool b, const FHitResult &Result);
	UFUNCTION()
	void ActorEndOverlap(UPrimitiveComponent* comp, AActor* OtherActor, class UPrimitiveComponent *C, int32 i);
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit);
#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent &e);
#endif
	/*End*/

	/*Entity Inetrface start*/
	//Lua
	virtual int GetLuaRef();
	virtual void SetLuaRef(int r);
	virtual void LuaUnRef();
	virtual void SetLuaClass(FString s);

	virtual void Remove();
	virtual int EntIndex();
	virtual void SetPos(FVector vec);
	virtual void SetAngles(FRotator ang);
	virtual FVector GetPos();
	virtual FRotator GetAngles();
	virtual void SetColor(FColor col);
	virtual FColor GetColor();

	//lib
	virtual void AddPhysicsObject(); //Works only in OnInit(), used to remove the synced physics system, ServerSide
	virtual FPhysObj *GetPhysicsObject();
	virtual void SetCollisionModel(ECollisionType collision); //ServerSide
	virtual ECollisionType GetCollisionModel();
	virtual void SetModel(FString path);
	virtual FString GetModel();
	virtual void SetMaterial(FString path); //No sync possible : UE4 does not allow c array replication
	virtual void SetSubMaterial(int32 index, FString path); //No sync possible : UE4 does not allow c array replication
	virtual FString GetMaterial();
	virtual FString GetSubMaterial(int32 index);
	virtual int32 GetSubMaterialsNum();
	virtual FString GetClass();
	//NW Vars (Future) (Using spacial macro)
	AUTO_NWVARS_HEADER();
	virtual EWaterLevel GetWaterLevel();
	/* Entity Interface end */	

	virtual void OnTick(); //Shared
	virtual void OnInit(); //Shared	
	virtual void OnPhysicsCollide(Entity *other); //ServerSide
	virtual void OnBeginOverlap(Entity *other); //ServerSide
	virtual void OnEndOverlap(Entity *other); //ServerSide

	//Init properties
	template <typename T>
	bool GetInitProperty(FString name, T &out);
	template <>
	bool GetInitProperty<float>(FString name, float &out)
	{
		for (int i = 0; i < InitProperties.Num(); i++) {
			if (InitProperties[i] == name) {
				out = FCString::Atof(*InitProperties[i].Value);
				return true;
			}
		}
		return false;
	}
	template <>
	bool GetInitProperty<FString>(FString name, FString &out)
	{
		for (int i = 0; i < InitProperties.Num(); i++) {
			if (InitProperties[i] == name) {
				out = InitProperties[i].Value;
				return true;
			}
		}
		return false;
	}
	template <>
	bool GetInitProperty<int>(FString name, int &out)
	{
		for (int i = 0; i < InitProperties.Num(); i++) {
			if (InitProperties[i] == name) {
				out = FCString::Atoi(*InitProperties[i].Value);
				return true;
			}
		}
		return false;
	}
	//End
};