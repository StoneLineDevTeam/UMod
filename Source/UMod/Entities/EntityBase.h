// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "EntityBase.generated.h"

enum EWaterLevel {
	FULL_SUBMERGED,
	HALF_SUBMERGED,
	NULL_SUMBERGED
};

UCLASS()
class UMOD_API AEntityBase : public AActor
{
	GENERATED_BODY()
	
	class UStaticMeshComponent *EntityModel;

private:
	UPROPERTY(Replicated)
	FVector DesiredPos;
	UPROPERTY(Replicated)
	FRotator DesiredRot;

	UPROPERTY(ReplicatedUsing = UpdateCollisionStatus)
	bool Collides = true;

	//Does this entity manages physics
	bool PhysEnabled = false;

	bool Initializing = false;

	FString CurMdl;

	UPROPERTY(ReplicatedUsing = UpdateClientMDL)
	FString ServerMDLSync;
	//UPROPERTY(ReplicatedUsing = UpdateClientMAT)
	FString* ServerMATSync;

	UPROPERTY(EditAnywhere)
	TMap<FString, FString> InitProperties;

	UFUNCTION()
	void UpdateClientMDL();
	UFUNCTION()
	void UpdateCollisionStatus();
	UFUNCTION()
	void UpdateClientMAT();

	float GravityScale = 1;
public:	
	AEntityBase();

	/*Begin AActor interface*/
	virtual void BeginPlay() override;	
	virtual void Tick(float DeltaSeconds) override;
	void AEntityBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty, FDefaultAllocator> & OutLifetimeProps) const;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor);
	virtual void NotifyActorEndOverlap(AActor* OtherActor);
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit);
	/*End*/

	/* Begin entity base lib */
	void SetPhysicsEnabled(bool b); //Works only in OnInit(), used to remove the synced physics system, ServerSide
	void SetCollisionEnabled(bool b); //ServerSide
	void SetModel(FString path);
	void SetGravityScale(float f); //ServerSide
	float GetGravityScale(); //ServerSide
	void SetMassScale(float f); //ServerSide
	float GetMassScale(); //ServerSide
	void AddForceCenter(FVector Force);
	void AddForceOffset(FVector Offset, FVector Force);
	void SetVelocity(FVector NewVel);
	FString GetModel();
	void Construct();
	void SetMaterial(FString path); //No sync possible : UE4 does not allow c array replication
	void SetSubMaterial(int32 index, FString path); //No sync possible : UE4 does not allow c array replication
	FString GetMaterial();
	FString GetSubMaterial(int32 index);
	int32 GetSubMaterialsNum();
	FString* GetInitProperty(FString name);
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
	virtual void OnInit(); //Shared
	virtual void OnPhysicsCollide(AEntityBase *other); //ServerSide
	virtual void OnBeginOverlap(AEntityBase *other); //ServerSide
	virtual void OnEndOverlap(AEntityBase *other); //ServerSide
};
