#pragma once
#include "Animation/AnimInstanceProxy.h"
#include "Animation/AnimInstance.h"
#include "NativePlayerAnimator.generated.h"

struct FNativeUModAnimProxy : FAnimInstanceProxy {
	virtual bool Evaluate(FPoseContext &Output);
};

UCLASS()
class UNativePlayerAnimator : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy();
	virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy);
};