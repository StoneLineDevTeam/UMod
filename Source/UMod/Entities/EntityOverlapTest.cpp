#include "UMod.h"
#include "EntityOverlapTest.h"

AEntityOverlapTest::AEntityOverlapTest()
{
	SetModel("RoundedCube");
}

void AEntityOverlapTest::OnInit()
{
	SetPhysicsEnabled(false);
	SetCollisions(ECollisionType::COLLISION_NONE);
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