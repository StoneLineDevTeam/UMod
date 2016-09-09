#pragma once

#include "Engine.h"
#include "ModuleManager.h"
#include "UnrealEd.h"

DECLARE_LOG_CATEGORY_EXTERN(UModEditor, All, All)

#define UMOD_EDIT_MSG_TITLE new FText(FText::FromString(TEXT("UMod - Unreal Editor")))
#define UMOD_EDIT_STANDARD_MSG(msgcontent) \
FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT(##msgcontent)), UMOD_EDIT_MSG_TITLE) \

#define SUBSTR(VarName, Str, Start, End) \
FString VarName = ##Str.Mid(##Start, (##End) - (##Start)) \

class FUModEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;	
};