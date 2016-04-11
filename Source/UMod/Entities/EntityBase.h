// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "EntityBase.generated.h"

enum ECollisionType {
	COLLISION_NONE, //No collisions
	COLLISION_PHYSICS, //Collides with everything used for physics simulation
	COLLISION_NOT_PLAYER, //Collides with everything but not player
	COLLISION_WORLD_ONLY //Collides only with non dynamic entities
};
enum EWaterLevel {
	FULL_SUBMERGED,
	HALF_SUBMERGED,
	NULL_SUMBERGED
};

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
class UMOD_API AEntityBase : public AActor
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

	//Does this entity manages physics
	bool PhysEnabled = false;

	bool Initializing = false;

	FString CurMdl;
#if WITH_EDITOR
	//FIX : Editor not updating model
	FString EditorCurMdl;
#endif
	
	UPROPERTY(EditAnywhere)
	TArray<FInitProperty> InitProperties;

	float GravityScale = 1;

	int LuaReference; //Yes starting LuaEntityBase !
	FString LuaClassName; //The lua class name
protected:
	UUModGameInstance *Game;

public:	
	AEntityBase();

	/*Begin AActor interface*/
	virtual void BeginPlay() override;	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty, FDefaultAllocator> & OutLifetimeProps) const;
	UFUNCTION()
	void ActorBeginOverlap(AActor* OtherActor, class UPrimitiveComponent *C, int32 i, bool b, const FHitResult &Result);
	UFUNCTION()
	void ActorEndOverlap(AActor* OtherActor, class UPrimitiveComponent *C, int32 i);
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit);
#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent &e);
#endif
	/*End*/

	/*Lua integration*/
	int GetLuaRef();
	void SetLuaRef(int r);
	void LuaUnRef();
	void SetLuaClass(FString s);
	/*End*/

	/* Begin entity base lib */
	void SetPhysicsEnabled(bool b); //Works only in OnInit(), used to remove the synced physics system, ServerSide
	void SetCollisionModel(ECollisionType collision); //ServerSide
	ECollisionType GetCollisionModel();
	void SetModel(FString path);
	void SetGravityScale(float f); //ServerSide
	float GetGravityScale(); //ServerSide
	void SetMassScale(float f); //ServerSide
	float GetMassScale(); //ServerSide
	void AddForceCenter(FVector Force); //ServerSide
	void AddForceOffset(FVector Offset, FVector Force); //ServerSide
	void SetVelocity(FVector NewVel);
	FString GetModel();
	void Construct();
	void SetMaterial(FString path); //No sync possible : UE4 does not allow c array replication
	void SetSubMaterial(int32 index, FString path); //No sync possible : UE4 does not allow c array replication
	FString GetMaterial();
	FString GetSubMaterial(int32 index);
	int32 GetSubMaterialsNum();
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
	void Freeze(); //ServerSide
	void Unfreeze(); //ServerSide
	//NW Vars (Future)
	void SetNWInt(FString id, int i);
	void SetNWString(FString id, FString str);
	void SetNWUnsined(FString id, uint32 i);
	void SetNWFloat(FString id, float f);
	int GetNWInt(FString id);
	FString GetNWString(FString id);
	uint32 GetNWUnsined(FString id);
	float GetNWFloat(FString id);
	void RemoveNWVar(FString id);
	/* End */

	virtual FString GetClass(); //Shared
	virtual void OnTick(); //Shared
	virtual void OnInit(); //Server
	virtual void OnClientInit(); //Client
	virtual void OnPhysicsCollide(AEntityBase *other); //ServerSide
	virtual void OnBeginOverlap(AEntityBase *other); //ServerSide
	virtual void OnEndOverlap(AEntityBase *other); //ServerSide
};
