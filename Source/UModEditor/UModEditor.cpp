#include "UModEditor.h"

#include "LevelEditor.h"

IMPLEMENT_GAME_MODULE(FUModEditorModule, UModEditor);

DEFINE_LOG_CATEGORY(UModEditor)

void FUModEditorModule::CompileAddonContent()
{
	
}

void FUModEditorModule::FillUModMenu(FMenuBuilder &MenuBuilder)
{
	MenuBuilder.BeginSection("UMod");
	MenuBuilder.AddMenuEntry(FText::FromString("Compile Addon Content"), FText::FromString("Compile Addon Content"), FSlateIcon::FSlateIcon(), FUIAction(FExecuteAction::CreateRaw(this, &FUModEditorModule::CompileAddonContent)));
	MenuBuilder.AddMenuEntry(FText::FromString("Cook UMod Content"), FText::FromString("Cook UMod Content"), FSlateIcon::FSlateIcon(), FUIAction(FExecuteAction::CreateRaw(this, &FUModEditorModule::CompileAddonContent)));
	MenuBuilder.EndSection();
}

void FUModEditorModule::CreateUModMenu(FMenuBarBuilder &MenuBuilder)
{
	MenuBuilder.AddPullDownMenu(FText::FromString("UMod"), FText::FromString("UMod"), FNewMenuDelegate::CreateRaw(this, &FUModEditorModule::FillUModMenu));
}

void FUModEditorModule::StartupModule()
{
	UE_LOG(UModEditor, Log, TEXT("Starting UMod Editor..."));

	TSharedRef<FExtender> Extender(new FExtender());

	Extender->AddMenuBarExtension("Help", EExtensionHook::After, NULL, FMenuBarExtensionDelegate::CreateRaw(this, &FUModEditorModule::CreateUModMenu));
	
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(Extender);
}

void FUModEditorModule::ShutdownModule()
{
	UE_LOG(UModEditor, Log, TEXT("Closing UMod Editor..."));
}