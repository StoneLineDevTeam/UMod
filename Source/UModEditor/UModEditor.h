#pragma once

#include "Engine.h"
#include "ModuleManager.h"
#include "UnrealEd.h"

DECLARE_LOG_CATEGORY_EXTERN(UModEditor, All, All)

class FUModEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void FillUModMenu(FMenuBuilder &MenuBuilder);
	void CreateUModMenu(FMenuBarBuilder &MenuBuilder);
	void CompileAddonContent();
};