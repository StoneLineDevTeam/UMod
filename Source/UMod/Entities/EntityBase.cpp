// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "EntityBase.h"
#include "UModGameInstance.h"

/*AActor base integration*/
AEntityBase::AEntityBase()
{
	Initializing = true;
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	EntityModel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EntityModel"));
}
void AEntityBase::BeginPlay()
{
	Super::BeginPlay();

	EntityModel->OnComponentBeginOverlap.AddDynamic(this, &AEntityBase::ActorBeginOverlap);
	EntityModel->OnComponentEndOverlap.AddDynamic(this, &AEntityBase::ActorEndOverlap);

	this->OnInit();

	if (PhysEnabled) {
		if (Role == ROLE_Authority) {
			EntityModel->WakeRigidBody();
			EntityModel->SetSimulatePhysics(true);
		} else {
			DisableComponentsSimulatePhysics();
			EntityModel->SetSimulatePhysics(false);
		}
	}

	Game = Cast<UUModGameInstance>(GetGameInstance());
}
void AEntityBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority) {
		if (PhysEnabled) {
			//TODO : Physics hack (gravity scale/maybe direction in future)
			if (GravityScale < 1 && GravityScale > 0) {
				APhysicsVolume *v = EntityModel->GetPhysicsVolume();
				float grav = -v->GetGravityZ() * EntityModel->GetMass() * (1 - GravityScale);
				FVector GravityVec = FVector(0, 0, grav);
				EntityModel->AddForce(GravityVec);
			}
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
		FVector lerped = FMath::Lerp(a1, b1, 0.5F);
		SetActorRotation(DesiredRot);
	}
	this->OnTick();
}
void AEntityBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty, FDefaultAllocator> & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEntityBase, DesiredPos);
	DOREPLIFETIME(AEntityBase, DesiredRot);
	DOREPLIFETIME(AEntityBase, CurCollisionProfile);
	DOREPLIFETIME(AEntityBase, ServerMDLSync);
	//DOREPLIFETIME(AEntityBase, ServerMATSync);
}
void AEntityBase::UpdateClientMDL()
{
	if (Role == ROLE_Authority) { return; }
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
	UE_LOG(UMod_Game, Warning, TEXT("Update Collision Status [Client]"));
	EntityModel->bGenerateOverlapEvents = false;
	ECollisionType t = (ECollisionType)(CurCollisionProfile - 1);
	if (t != ECollisionType::COLLISION_NONE) {
		EntityModel->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		switch (t) {
		case ECollisionType::COLLISION_NOT_PLAYER:
			EntityModel->SetCollisionProfileName("OverlapOnlyPawn");
		case ECollisionType::COLLISION_WORLD_ONLY:
			EntityModel->SetCollisionProfileName("OverlapAllDynamic");
		}
	} else {
		EntityModel->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}
void AEntityBase::Construct()
{
	EntityModel->SetMobility(EComponentMobility::Movable);
	SetRootComponent(EntityModel);

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
	}
	Initializing = false;
}
void AEntityBase::ActorBeginOverlap(AActor* OtherActor, UPrimitiveComponent *C, int32 i, bool b, const FHitResult &Result)
{
	UE_LOG(UMod_Game, Warning, TEXT("Start overlap !"));
	if (OtherActor->IsA(AEntityBase::StaticClass())) {
		AEntityBase *Ent = Cast<AEntityBase>(OtherActor);
		OnBeginOverlap(Ent);
	}
}
void AEntityBase::ActorEndOverlap(AActor* OtherActor, UPrimitiveComponent *C, int32 i)
{
	UE_LOG(UMod_Game, Warning, TEXT("End overlap !"));
	if (OtherActor->IsA(AEntityBase::StaticClass())) {
		AEntityBase *Ent = Cast<AEntityBase>(OtherActor);
		OnEndOverlap(Ent);
	}
}
void AEntityBase::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{

}
void AEntityBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	LuaUnRef();
}
#if WITH_EDITOR
//FIX : Editor not updating model
void AEntityBase::PostEditChangeChainProperty(struct FPropertyChangedChainEvent &e)
{
	FString EditorNewMdl;
	bool b = GetInitProperty<FString>("Model", EditorNewMdl);
	if (EditorCurMdl != EditorNewMdl && b) {
		EditorCurMdl = EditorNewMdl;
		SetModel(EditorCurMdl);
		UE_LOG(UMod_Maps, Warning, TEXT("Test"));
	}
}
#endif
/*End*/


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

void AEntityBase::SetCollisionModel(ECollisionType collision)
{
	if (Role != ROLE_Authority) { return; }
	if (collision != ECollisionType::COLLISION_NONE) {
		EntityModel->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		EntityModel->bGenerateOverlapEvents = false;
		EntityModel->SetNotifyRigidBodyCollision(true);
		switch (collision) {
		case ECollisionType::COLLISION_NOT_PLAYER:
			EntityModel->SetCollisionProfileName("OverlapOnlyPawn");
			EntityModel->bGenerateOverlapEvents = true;
		case ECollisionType::COLLISION_WORLD_ONLY:
			EntityModel->SetCollisionProfileName("OverlapAllDynamic");
			EntityModel->bGenerateOverlapEvents = true;
		case ECollisionType::COLLISION_PHYSICS:
			EntityModel->SetCollisionProfileName("BlockAll");
		}
	} else {
		EntityModel->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		EntityModel->SetCollisionProfileName("OverlapAll");
		EntityModel->bGenerateOverlapEvents = true;
		EntityModel->SetNotifyRigidBodyCollision(false);
	}
	CurCollisionProfile = (uint8)collision + 1;
}

ECollisionType AEntityBase::GetCollisionModel()
{
	return (ECollisionType)(CurCollisionProfile - 1);
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

int AEntityBase::GetLuaRef()
{
	return LuaReference;
}

void AEntityBase::SetLuaRef(int r)
{
	LuaReference = r;
}

void AEntityBase::SetLuaClass(FString s)
{
	LuaClassName = s;
}

void AEntityBase::LuaUnRef()
{
	Game->Lua->Lua->UnRef(LuaReference);
	LuaReference = LUA_NOREF;
}

FString AEntityBase::GetClass()
{
	if (!LuaClassName.IsEmpty()) {
		return LuaClassName;
	}
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
