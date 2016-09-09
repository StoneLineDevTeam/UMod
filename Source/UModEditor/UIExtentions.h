#pragma once
#include "UModEditor.h"

enum ELuaDocType {
	LUA_LIBRARY,
	LUA_TYPE,
	LUA_BASETYPE,
	LUA_OTHER
};

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
	void LuaMenu(FMenuBuilder &MenuBuilder);
	void PrepareLuaDocs();
	void GenerateLuaDocs();
};