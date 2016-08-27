#include "UModEditor.h"
#include "UModEditorEngine.h"
#include "UIExtentions.h"
#include "VMTConverter.h"
#include "LevelEditor.h"
#include "DesktopPlatformModule.h"
#include "MainFrame.h"
#include "ContentBrowserModule.h"

void UIExtentions::CompileAddonContent()
{

}
void UIExtentions::ConvertSingleFile()
{
	IContentBrowserSingleton& Browser = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get();
	TArray<UObject*> SyncObjs;

	IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	const TSharedPtr<SWindow>& Window = MainFrameModule.GetParentWindow();
	TArray<FString> strs;
	bool b = FDesktopPlatformModule::Get()->OpenFileDialog(Window->GetNativeWindow()->GetOSWindowHandle(), "Select file to convert", "", "", ".vmt", EFileDialogFlags::None, strs);
	if (b) {
		UE_LOG(UModEditor, Log, TEXT("-----> VMT Conversion Start <-----"));
		UE_LOG(UModEditor, Log, TEXT("Input file : %s"), *strs[0]);
		UMaterialInstanceConstant *Asset = VMTConverter::StartFileConversion(strs[0]);
		if (Asset != NULL) {
			SyncObjs.Add(Asset);
			Browser.SyncBrowserToAssets(SyncObjs);
		}
		UE_LOG(UModEditor, Log, TEXT("-----> VMT Conversion End <-----"));
	}
	else {
		UE_LOG(UModEditor, Error, TEXT("Operation aborted by user."));
	}
}
void UIExtentions::ConvertEntireFolder()
{
	IMainFrameModule& MainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	const TSharedPtr<SWindow>& Window = MainFrameModule.GetParentWindow();
	FString ToConvertFolder;
	bool b = FDesktopPlatformModule::Get()->OpenDirectoryDialog(Window->GetNativeWindow()->GetOSWindowHandle(), "Select folder to convert", "", ToConvertFolder);
	if (b) {
		UE_LOG(UModEditor, Log, TEXT("-----> VMT Conversion Start <-----"));
		UE_LOG(UModEditor, Log, TEXT("Input folder : %s"), *ToConvertFolder);
		VMTConverter::StartFolderConversion(ToConvertFolder);
		UE_LOG(UModEditor, Log, TEXT("-----> VMT Conversion End <-----"));
	}
	else {
		UE_LOG(UModEditor, Error, TEXT("Operation aborted by user."));
	}
}
void UIExtentions::OpenLauncher()
{
	if (!FPlatformProcess::IsApplicationRunning(TEXT("EpicGamesLauncher")) && !FPlatformProcess::IsApplicationRunning(TEXT("EpicGamesLauncher-Mac-Shipping"))) {
		FDesktopPlatformModule::Get()->OpenLauncher(FOpenLauncherOptions(false, TEXT("")));
	}
}
void UIExtentions::ConvertMaterialMenu(FMenuBuilder &MenuBuilder)
{
	MenuBuilder.AddMenuEntry(FText::FromString("Convert Single File"), FText::FromString("Single file conversion"), FSlateIcon::FSlateIcon(), FUIAction(FExecuteAction::CreateRaw(this, &UIExtentions::ConvertSingleFile)));
	MenuBuilder.AddMenuEntry(FText::FromString("Convert Entire Folder"), FText::FromString("Multiple files conversion (automatic, recursive)"), FSlateIcon::FSlateIcon(), FUIAction(FExecuteAction::CreateRaw(this, &UIExtentions::ConvertEntireFolder)));
}
void UIExtentions::FillUModMenu(FMenuBuilder &MenuBuilder)
{
	MenuBuilder.BeginSection("UMod");
	MenuBuilder.AddMenuEntry(FText::FromString("Compile Addon Content"), FText::FromString("Compile Addon Content"), FSlateIcon::FSlateIcon(), FUIAction(FExecuteAction::CreateRaw(this, &UIExtentions::CompileAddonContent)));
	MenuBuilder.AddSubMenu(FText::FromString("Material Converter"), FText::FromString("Convert VMT file representation into UMod Lua ready materials"), FNewMenuDelegate::CreateRaw(this, &UIExtentions::ConvertMaterialMenu));
	MenuBuilder.AddMenuEntry(FText::FromString("Open Launcher"), FText::FromString("Opens the Epic Games Launcher"), FSlateIcon::FSlateIcon(), FUIAction(FExecuteAction::CreateRaw(this, &UIExtentions::OpenLauncher)));
	MenuBuilder.EndSection();
}
void UIExtentions::CreateUModMenu(FMenuBarBuilder &MenuBuilder)
{
	MenuBuilder.AddPullDownMenu(FText::FromString("UMod"), FText::FromString("UMod"), FNewMenuDelegate::CreateRaw(this, &UIExtentions::FillUModMenu));
}
void UIExtentions::BuildExtentions(UUModEditorEngine *Engine) {
	GEngine = Engine;

	TSharedRef<FExtender> Extender(new FExtender());

	Extender->AddMenuBarExtension("Help", EExtensionHook::After, NULL, FMenuBarExtensionDelegate::CreateRaw(this, &UIExtentions::CreateUModMenu));

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(Extender);
}