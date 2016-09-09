#include "UMod.h"
#include "EntityOverlapTest.h"

AEntityOverlapTest::AEntityOverlapTest()
{
	//SetModel("UMod:RoundedCube");
}

void AEntityOverlapTest::OnInit()
{	
	SetCollisionModel(ECollisionType::COLLISION_NONE);
	SetModel("UMod:RoundedCube");
}

void AEntityOverlapTest::OnBeginOverlap(Entity *other)
{
	UE_LOG(UMod_Game, Warning, TEXT("An entity started to overlap"));
	SetModel("UMod:CubeMesh");
}

void AEntityOverlapTest::OnEndOverlap(Entity *other)
{
	UE_LOG(UMod_Game, Warning, TEXT("An entity ended to overlap"));
	SetModel("UMod:RoundedCube");
}

FString AEntityOverlapTest::GetClass()
{
	return "OverlapTest";
}

DEFINE_ENTITY(OverlapTest, AEntityOverlapTest)