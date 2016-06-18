#include "UModEditor.h"

#include "LevelEditor.h"

IMPLEMENT_GAME_MODULE(FUModEditorModule, UModEditor);

DEFINE_LOG_CATEGORY(UModEditor)

static void FillUModMenu(FMenuBuilder& MenuBuilder)
{

}

void FUModEditorModule::StartupModule()
{
	UE_LOG(UModEditor, Log, TEXT("Starting UMod Editor..."));

	TSharedRef<FExtender> Extender(new FExtender());

	Extender->AddMenuExtension("EditHistory", EExtensionHook::After, NULL, FMenuExtensionDelegate::CreateStatic(&FillUModMenu));
	
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(Extender);
}

void FUModEditorModule::ShutdownModule()
{
	UE_LOG(UModEditor, Log, TEXT("Closing UMod Editor..."));
}