// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "EntityBase.h"

/*AActor base integration*/
AEntityBase::AEntityBase()
{
	Initializing = true;
	PrimaryActorTick.bCanEverTick = true;

	EntityModel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EntityModel"));
}
void AEntityBase::BeginPlay()
{
	Super::BeginPlay();

	if (PhysEnabled) {
		if (Role == ROLE_Authority) {
			EntityModel->WakeRigidBody();
			EntityModel->SetSimulatePhysics(true);
		} else {
			DisableComponentsSimulatePhysics();
			EntityModel->SetSimulatePhysics(false);
		}
	}
	this->OnInit();
}
void AEntityBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PhysEnabled) {
		if (Role == ROLE_Authority) {
			//TODO : Physics hack (gravity scale/maybe direction in future)
			if (GravityScale < 1 && GravityScale > 0) {
				APhysicsVolume *v = EntityModel->GetPhysicsVolume();
				float grav = -v->GetGravityZ() * EntityModel->GetMass() * (1 - GravityScale);
				FVector GravityVec = FVector(0, 0, grav);
				EntityModel->AddForce(GravityVec);
			}

			//Send physx simulation data		
			DesiredPos = GetActorLocation();
			DesiredRot = GetActorRotation();
		} else {
			//Interpolate between cur pos and new pos received from server
			FVector a = GetActorLocation();
			FVector b = DesiredPos;
			FVector newPos = FMath::Lerp(a, b, 0.5F);
			SetActorLocation(newPos);

			FVector a1 = GetActorRotation().Vector();
			FVector b1 = DesiredRot.Vector();
			FVector lerped = FMath::Lerp(a1, b1, 0.25F);
			SetActorRotation(DesiredRot);
		}
	}
	this->OnTick();
}
void AEntityBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty, FDefaultAllocator> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEntityBase, DesiredPos);
	DOREPLIFETIME(AEntityBase, DesiredRot);
	DOREPLIFETIME(AEntityBase, ServerMDLSync);
	//DOREPLIFETIME(AEntityBase, ServerMATSync);
	DOREPLIFETIME(AEntityBase, Collides);
}
void AEntityBase::UpdateClientMDL()
{
	UStaticMesh *model = LoadObjFromPath<UStaticMesh>(*FString("/Game/Models/" + ServerMDLSync));
	EntityModel->SetStaticMesh(model);
	CurMdl = ServerMDLSync;
	ServerMATSync = new FString[GetSubMaterialsNum()];
}
void AEntityBase::UpdateClientMAT()
{
	for (int i = 0; i < GetSubMaterialsNum(); i++) {
		FString path = ServerMATSync[i];
		UMaterialInterface *mat = LoadObjFromPath<UMaterialInterface>(*FString("/Game/" + path));
		EntityModel->SetMaterial(i, mat);
	}
}
void AEntityBase::UpdateCollisionStatus()
{
	if (Collides) {
		EntityModel->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	} else {
		EntityModel->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
void AEntityBase::Construct()
{
	EntityModel->SetMobility(EComponentMobility::Movable);
	SetRootComponent(EntityModel);

	bReplicates = true;

	if (PhysEnabled) {
		EntityModel->bGenerateOverlapEvents = true;
		EntityModel->SetNotifyRigidBodyCollision(true);
		if (Role == ROLE_Authority) {
			EntityModel->SetSimulatePhysics(true);
			EntityModel->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		} else {
			DisableComponentsSimulatePhysics();
			EntityModel->SetSimulatePhysics(false);
			EntityModel->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
	} else {
		EntityModel->bGenerateOverlapEvents = false;
		EntityModel->SetNotifyRigidBodyCollision(false);
	}
	Initializing = false;
}
void AEntityBase::NotifyActorBeginOverlap(AActor* OtherActor)
{

}
void AEntityBase::NotifyActorEndOverlap(AActor* OtherActor)
{

}
void AEntityBase::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{

}
/*End*/

FString* AEntityBase::GetInitProperty(FString name)
{
	return InitProperties.Find(name);
}

void AEntityBase::SetPhysicsEnabled(bool b)
{
	if (Role != ROLE_Authority) { return; }
	PhysEnabled = b;
}

void AEntityBase::SetGravityScale(float f)
{
	if (Role != ROLE_Authority) { return; }
	GravityScale = f;
	if (GravityScale == 0) {
		EntityModel->SetEnableGravity(false);
	} else if (GravityScale == 1) {
		EntityModel->SetEnableGravity(true);
	}
}

float AEntityBase::GetGravityScale()
{
	if (Role != ROLE_Authority) { return 0.0F; }
	return GravityScale;
}

void AEntityBase::SetMassScale(float f)
{
	if (Role != ROLE_Authority) { return; }
	EntityModel->SetMassScale(NAME_None, f);
}

float AEntityBase::GetMassScale()
{
	if (Role != ROLE_Authority) { return 0.0F; }
	return EntityModel->GetMassScale(NAME_None);
}

void AEntityBase::SetModel(FString path)
{
	if (Role == ROLE_Authority && !Initializing) {
		ServerMDLSync = path;
	}
	UStaticMesh *model = LoadObjFromPath<UStaticMesh>(*FString("/Game/Models/" + path));
	EntityModel->SetStaticMesh(model);
	CurMdl = path;

	ServerMATSync = new FString[GetSubMaterialsNum()];
}

FString AEntityBase::GetModel()
{
	return "Models/" + CurMdl;
}

void AEntityBase::SetCollisionEnabled(bool b)
{
	if (Role != ROLE_Authority) { return; }
	if (b) {
		EntityModel->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		EntityModel->bGenerateOverlapEvents = false;
		EntityModel->SetNotifyRigidBodyCollision(true);
	} else {
		EntityModel->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		EntityModel->bGenerateOverlapEvents = true;
		EntityModel->SetNotifyRigidBodyCollision(false);
	}	
}

void AEntityBase::SetMaterial(FString path)
{
	UMaterialInterface *mat = LoadObjFromPath<UMaterialInterface>(*FString("/Game/" + path));
	for (int i = 0; i < GetSubMaterialsNum(); i++) {
		EntityModel->SetMaterial(i, mat);
		if (Role == ROLE_Authority) {
			ServerMATSync[i] = path;
		}
	}
}

void AEntityBase::SetSubMaterial(int32 index, FString path)
{
	if (GetSubMaterialsNum() < index) { return; }
	EntityModel->SetMaterial(index, LoadObjFromPath<UMaterialInterface>(*FString("/Game/" + path)));
	if (Role == ROLE_Authority) {
		ServerMATSync[index] = path;
	}
}

FString AEntityBase::GetMaterial()
{
	return GetObjPath(EntityModel->GetMaterial(0));
}

FString AEntityBase::GetSubMaterial(int32 index)
{
	if (GetSubMaterialsNum() < index) { return FString(); }
	return GetObjPath(EntityModel->GetMaterial(index));
}

int32 AEntityBase::GetSubMaterialsNum()
{
	return EntityModel->GetNumMaterials();
}

void AEntityBase::Freeze()
{
	if (Role != ROLE_Authority) { return; }
	EntityModel->SetSimulatePhysics(false);
	PhysEnabled = false;
}

void AEntityBase::Unfreeze()
{
	if (Role != ROLE_Authority) { return; }
	EntityModel->SetSimulatePhysics(true);
	PhysEnabled = true;
}


FString AEntityBase::GetClass()
{
	return "NULL";
}
void AEntityBase::OnTick()
{

}
void AEntityBase::OnInit()
{

}
void AEntityBase::OnPhysicsCollide(AEntityBase *other)
{

}
void AEntityBase::OnBeginOverlap(AEntityBase *other)
{

}
void AEntityBase::OnEndOverlap(AEntityBase *other)
{

}
