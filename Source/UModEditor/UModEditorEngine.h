#pragma once
#include "UModEditor.h"
#include "UnrealEd.h"
#include "UIExtentions.h"
#include "UModEditorEngine.generated.h"

UCLASS()
class UUModEditorEngine : public UUnrealEdEngine
{
	GENERATED_BODY()

public:	
	virtual void Init(IEngineLoop* InEngineLoop) override;
	virtual void PlayInEditor(UWorld* InWorld, bool bInSimulateInEditor);
private:
	UIExtentions *Exts;
};