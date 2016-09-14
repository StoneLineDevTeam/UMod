#include "UModEditor.h"

#include "UnrealEd.h"
#include "UModEditorEngine.h"

IMPLEMENT_GAME_MODULE(FUModEditorModule, UModEditor);

DEFINE_LOG_CATEGORY(UModEditor)

void FUModEditorModule::StartupModule()
{
	UE_LOG(UModEditor, Log, TEXT("Starting UMod Editor..."));
	//Create a message log for UMod Editor
	//OpenMessageLog to run the message log window with a specified category
	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	FMessageLogInitializationOptions InitOptions;
	InitOptions.bShowPages = true;
	MessageLogModule.RegisterLogListing("UModEditor", FText::FromString("UModEditor"), InitOptions);

	/*
	IPlacementModeModule& PlacementModeModule = IPlacementModeModule::Get();

	FPlacementCategoryInfo CategoryInfo(LOCTEXT("PlacementMode_Geometry", "Geometry"), "Geometry", TEXT("PMGeometry"), 35);
	CategoryInfo.CustomGenerator = []() -> TSharedRef<SWidget> { return SNew(SBspPalette); };
	PlacementModeModule.RegisterPlacementCategory(CategoryInfo);
	*/
}

void FUModEditorModule::ShutdownModule()
{
	UE_LOG(UModEditor, Log, TEXT("Closing UMod Editor..."));
	if (FModuleManager::Get().IsModuleLoaded("MessageLog"))
	{
		//Unregister UMod Editor message log
		FMessageLogModule& MessageLogModule = FModuleManager::GetModuleChecked<FMessageLogModule>("MessageLog");
		MessageLogModule.UnregisterLogListing("UModEditor");
	}
}