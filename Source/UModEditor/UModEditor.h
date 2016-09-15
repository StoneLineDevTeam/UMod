#pragma once

#include "Engine.h"
#include "ModuleManager.h"
#include "UnrealEd.h"
#include "MessageLogModule.h"

DECLARE_LOG_CATEGORY_EXTERN(UModEditor, All, All)

#define UMOD_EDIT_MSG_TITLE new FText(FText::FromString(TEXT("UMod - Unreal Editor")))
#define UMOD_EDIT_STANDARD_MSG(msgcontent) \
FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(TEXT(msgcontent)), UMOD_EDIT_MSG_TITLE) \

#define SUBSTR(VarName, Str, Start, End) \
FString VarName = Str.Mid(Start, (End) - (Start)) \

#define LEVEL_INFO 0
#define LEVEL_ERROR 1
#define LEVEL_WARNING 2
#define LEVEL_CRITICAL 3

FORCEINLINE void LogEditorMessage(int i, FString str)
{
	EMessageSeverity::Type t = EMessageSeverity::Info;
	switch (i) {
	case LEVEL_INFO:
		t = EMessageSeverity::Info;
		break;
	case LEVEL_WARNING:
		t = EMessageSeverity::Warning;
		break;
	case LEVEL_ERROR:
		t = EMessageSeverity::Error;
		break;
	case LEVEL_CRITICAL:
		t = EMessageSeverity::CriticalError;
		break;
	}
	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	TSharedRef<IMessageLogListing> lst = MessageLogModule.GetLogListing("UModEditor");
	TSharedRef<FTokenizedMessage> msg = FTokenizedMessage::Create(t, FText::FromString(str));
	lst->AddMessage(msg, false);
}

class FUModEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;	
};