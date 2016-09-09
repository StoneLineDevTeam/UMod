// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "EntityBase.h"
#include "UModGameInstance.h"
#include "Game/UModGameMode.h"
#include "AssertionMacros.h"

/*AActor base integration*/
AEntityBase::AEntityBase() : Super()
{
	Initializing = true;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;
	SetActorTickEnabled(true);
	
	bReplicates = true;
	bAlwaysRelevant = true;

	EntityModel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EntityModel"));
	SetRootComponent(EntityModel);

	LuaReference = LUA_NOREF;
}

void AEntityBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (GIsEditor) { return; }
	
	Game = Cast<UUModGameInstance>(GetGameInstance());

	this->OnInit();

	EntityModel->SetMobility(EComponentMobility::Movable);

	if (Role == ROLE_Authority) {
		EntityModel->bGenerateOverlapEvents = true;
		EntityModel->SetNotifyRigidBodyCollision(true);
		EntityModel->OnComponentBeginOverlap.AddDynamic(this, &AEntityBase::ActorBeginOverlap);
		EntityModel->OnComponentEndOverlap.AddDynamic(this, &AEntityBase::ActorEndOverlap);
	}

	if (PhysObj != NULL) {
		if (Role == ROLE_Authority) {
			EntityModel->WakeRigidBody();
			EntityModel->SetSimulatePhysics(true);
		}
		else {
			DisableComponentsSimulatePhysics();
			EntityModel->SetSimulatePhysics(false);
		}
	}

	Initializing = false;
}

void AEntityBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (Role == ROLE_Authority) {
		UE_LOG(UMod_Game, Log, TEXT("test"));
		AUModGameMode *gm = Cast<AUModGameMode>(GetWorld()->GetAuthGameMode());
		gm->OnEntitySpawn(this);
	}	
}
void AEntityBase::Tick(float DeltaTime)
{
	UMOD_STAT(PHYSICSSync);

	Super::Tick(DeltaTime);
	
	if (Role == ROLE_Authority) {
		if (PhysObj != NULL) {
			PhysObj->UpdateObj();
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
	AUTO_NWVARS_REP_CODE(AEntityBase);
	//DOREPLIFETIME(AEntityBase, ServerMATSync);
}
void AEntityBase::UpdateClientMDL()
{
	if (Role == ROLE_Authority) { return; }
	FString realPath;
	EResolverResult res = UUModAssetsManager::Instance->ResolveAsset(ServerMDLSync, EUModAssetType::MODEL, realPath);
	if (res != EResolverResult::SUCCESS) {
		return;
	}
	UE_LOG(UMod_Game, Log, TEXT("[DEBUG]EntityBase->ServerMDLSync : %s"), *realPath);
	UStaticMesh *model = LoadObjFromPath<UStaticMesh>(*realPath);
	EntityModel->SetStaticMesh(model);
	CurMdl = ServerMDLSync;
	ServerMATSync = new FString[GetSubMaterialsNum()];
}
void AEntityBase::UpdateClientMAT()
{
	if (Role == ROLE_Authority) { return; }
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
#undef dynamic_cast
void AEntityBase::ActorBeginOverlap(UPrimitiveComponent* comp, AActor* OtherActor, UPrimitiveComponent *C, int32 i, bool b, const FHitResult &Result)
{	
	Entity *ent = UModCasts::FromActor(OtherActor);
	if (ent != NULL) {
		OnBeginOverlap(ent);
	}
}
void AEntityBase::ActorEndOverlap(UPrimitiveComponent* comp, AActor* OtherActor, UPrimitiveComponent *C, int32 i)
{
	Entity *ent = UModCasts::FromActor(OtherActor);
	if (ent != NULL) {
		OnEndOverlap(ent);
	}
}
void AEntityBase::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{

}
void AEntityBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	LuaUnRef();
}
//FIX : Editor not updating model
#if WITH_EDITOR
void AEntityBase::PostEditChangeChainProperty(struct FPropertyChangedChainEvent &e)
{
	FString EditorNewMdl;
	bool b = GetInitProperty<FString>("Model", EditorNewMdl);
	if (EditorCurMdl != EditorNewMdl && b) {
		EditorCurMdl = EditorNewMdl;
		if (Game == NULL) {
			if (GetGameInstance() != NULL) {
				Game = Cast<UUModGameInstance>(GetGameInstance());
			} else {
				Game = NewObject<UUModGameInstance>();
			}
		}
		if (Game->AssetsManager == NULL) {
			Game->AssetsManager = new UUModAssetsManager();
		}
		SetModel(EditorCurMdl);
		MarkComponentsRenderStateDirty();
		UE_LOG(UMod_Maps, Warning, TEXT("Test"));
	}
}
#endif
/*End*/


void AEntityBase::AddPhysicsObject()
{
	if (Role != ROLE_Authority) { return; }
	PhysObj = new FPhysObj(EntityModel);
}

FPhysObj *AEntityBase::GetPhysicsObject()
{
	return PhysObj;
}

void AEntityBase::SetModel(FString path)
{	
	FString realPath;
	EResolverResult res = Game->AssetsManager->ResolveAsset(path, EUModAssetType::MODEL, realPath);
	if (res != EResolverResult::SUCCESS) {
		return;
	}
	if (Role == ROLE_Authority && !Initializing) {
		ServerMDLSync = path;
	}
	UStaticMesh *model = LoadObjFromPath<UStaticMesh>(*realPath);
	EntityModel->SetStaticMesh(model);
	CurMdl = path;

	ServerMATSync = new FString[GetSubMaterialsNum()];
}

FString AEntityBase::GetModel()
{
	if (Role == ROLE_Authority) {
		return CurMdl;
	} else {
		return ServerMDLSync;
	}
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
	FString realPath;
	EResolverResult res = Game->AssetsManager->ResolveAsset(realPath, EUModAssetType::MATERIAL, path);
	if (res != EResolverResult::SUCCESS) {
		return;
	}
	UMaterialInterface *mat = LoadObjFromPath<UMaterialInterface>(*realPath);
	for (int i = 0; i < GetSubMaterialsNum(); i++) {
		EntityModel->SetMaterial(i, mat);
		if (Role == ROLE_Authority) {
			ServerMATSync[i] = realPath;
		}
	}
}

void AEntityBase::SetSubMaterial(int32 index, FString path)
{
	if (GetSubMaterialsNum() < index) { return; }
	FString realPath;
	EResolverResult res = Game->AssetsManager->ResolveAsset(realPath, EUModAssetType::MATERIAL, path);
	if (res != EResolverResult::SUCCESS) {
		return;
	}
	UMaterialInterface *mat = LoadObjFromPath<UMaterialInterface>(*realPath);
	EntityModel->SetMaterial(index, mat);
	if (Role == ROLE_Authority) {
		ServerMATSync[index] = realPath;
	}
}

FString AEntityBase::GetMaterial()
{
	return "";//GetObjPath(EntityModel->GetMaterial(0));
}

FString AEntityBase::GetSubMaterial(int32 index)
{
	if (GetSubMaterialsNum() < index) { return FString(); }
	return "";//GetObjPath(EntityModel->GetMaterial(index));
}

int32 AEntityBase::GetSubMaterialsNum()
{
	return EntityModel->GetNumMaterials();
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

void AEntityBase::Remove()
{
	Destroy();
}

void AEntityBase::SetPos(FVector vec)
{
	SetActorLocation(vec);
}

void AEntityBase::SetAngles(FRotator ang)
{
	SetActorRotation(ang);
}

FVector AEntityBase::GetPos()
{
	return GetActorLocation();
}

FRotator AEntityBase::GetAngles()
{
	return GetActorRotation();
}

void AEntityBase::SetColor(FColor col)
{
	//I need my material utilities for that
}

FColor AEntityBase::GetColor()
{
	return FColor(0, 0, 0); //I need my material utilities for that
}

AUTO_NWVARS_BODY(AEntityBase)

EWaterLevel AEntityBase::GetWaterLevel()
{
	return EWaterLevel::NULL_SUMBERGED;
}

int AEntityBase::EntIndex()
{
	return GetUniqueID();
}

void AEntityBase::OnTick()
{

}
void AEntityBase::OnInit()
{

}
void AEntityBase::OnPhysicsCollide(Entity *other)
{

}
void AEntityBase::OnBeginOverlap(Entity *other)
{

}
void AEntityBase::OnEndOverlap(Entity *other)
{

}