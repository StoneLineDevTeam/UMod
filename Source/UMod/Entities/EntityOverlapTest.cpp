#include "UMod.h"
#include "EntityOverlapTest.h"

AEntityOverlapTest::AEntityOverlapTest()
{
	SetModel("RoundedCube");
}

void AEntityOverlapTest::OnInit()
{
	SetPhysicsEnabled(false);
	SetCollisionModel(ECollisionType::COLLISION_NONE);
	Construct();
}

void AEntityOverlapTest::OnBeginOverlap(AEntityBase *other)
{
	UE_LOG(UMod_Game, Warning, TEXT("An entity started to overlap"));
	SetModel("CubeMesh");
}

void AEntityOverlapTest::OnEndOverlap(AEntityBase *other)
{
	UE_LOG(UMod_Game, Warning, TEXT("An entity ended to overlap"));
	SetModel("RoundedCube");
}

FString AEntityOverlapTest::GetClass()
{
	return "OverlapTest";
}

DEFINE_ENTITY(OverlapTest, AEntityOverlapTest::StaticClass())