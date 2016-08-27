#include "UModEditor.h"

#include "UnrealEd.h"
#include "UModEditorEngine.h"

IMPLEMENT_GAME_MODULE(FUModEditorModule, UModEditor);

DEFINE_LOG_CATEGORY(UModEditor)

void FUModEditorModule::StartupModule()
{
	UE_LOG(UModEditor, Log, TEXT("Starting UMod Editor..."));
}

void FUModEditorModule::ShutdownModule()
{
	UE_LOG(UModEditor, Log, TEXT("Closing UMod Editor..."));
}