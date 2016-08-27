#pragma once
#include "UModEditor.h"

class UUModEditorEngine;

class UIExtentions {
private:
	UUModEditorEngine *GEngine;
public:
	void BuildExtentions(UUModEditorEngine *Engine);
protected:
	void FillUModMenu(FMenuBuilder &MenuBuilder);
	void CreateUModMenu(FMenuBarBuilder &MenuBuilder);
	void CompileAddonContent();
	void ConvertMaterialMenu(FMenuBuilder &MenuBuilder);
	void ConvertSingleFile();
	void ConvertEntireFolder();
	void OpenLauncher();
};