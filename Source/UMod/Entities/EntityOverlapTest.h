#pragma once
#include "EntityBase.h"
#include "EntityOverlapTest.generated.h"

UCLASS()
class AEntityOverlapTest : public AEntityBase {
	GENERATED_BODY()

	AEntityOverlapTest();
public:	
	virtual void OnInit();
	virtual void OnBeginOverlap(AEntityBase *other);
	virtual void OnEndOverlap(AEntityBase *other);
	virtual FString GetClass();
};

//DEFINE_ENTITY("OverlapTest", AEntityOverlapTest::StaticClass())