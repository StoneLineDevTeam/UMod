#include "UMod.h"
#include "NativePlayerAnimator.h"

bool FNativeUModAnimProxy::Evaluate(FPoseContext &Output)
{
	return FAnimInstanceProxy::Evaluate(Output); //That will be where I will allow Lua to change bone transforms on animated Skeletons
}

FAnimInstanceProxy* UNativePlayerAnimator::CreateAnimInstanceProxy()
{
	return new FNativeUModAnimProxy();
}

void UNativePlayerAnimator::DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy)
{
	delete InProxy;
}