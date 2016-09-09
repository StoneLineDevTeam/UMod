#pragma once
#include "EntityBase.h"
#include "EntityOverlapTest.generated.h"

UCLASS()
class AEntityOverlapTest : public AEntityBase {
	GENERATED_BODY()

	AEntityOverlapTest();
public:	
	virtual void OnInit();
	virtual void OnBeginOverlap(Entity *other);
	virtual void OnEndOverlap(Entity *other);
	virtual FString GetClass();
};