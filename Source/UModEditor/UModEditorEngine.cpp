#include "UModEditor.h"
#include "UModEditorEngine.h"

void UUModEditorEngine::Init(IEngineLoop* InEngineLoop)
{
	GIsDemoMode = true;
	Super::Init(InEngineLoop);
	GIsDemoMode = false;

	Exts = new UIExtentions();
	Exts->BuildExtentions(this);

	//Forcing content cooking on specific folder :
	//Engine/Source/Editor/MainFrame/Private/Frame/MainFrameActions.cpp line 367 to 376	
	//--> Require custom implementation <--	
	//https://docs.unrealengine.com/latest/INT/API/Runtime/Core/Misc/FMonitoredProcess/index.html Creates the UAT Task
	//https://docs.unrealengine.com/latest/INT/API/Runtime/Slate/Widgets/Notifications/FNotificationInfo/index.html Creates the cool Slate notification on the editor
	//Use FSlateNotificationManager::Get().AddNotification to add the notification	
	//On windows URL = EngineDir/Build/BatchFiles/RunUAT.bat
	//On mac URL = EngineDir/Build/BatchFiles/RunUAT.command
	//On Linux URL = EngineDir/Build/BatchFiles/RunUAT.sh
	//Executable names :
	//For windows : cmd.exe
	//For Linux : /bin/bash
	//For mac : /bin/sh
}

void UUModEditorEngine::PlayInEditor(UWorld* InWorld, bool bInSimulateInEditor)
{	
	UMOD_EDIT_STANDARD_MSG("You can not play UMod in PIE for compatibility issues with LuaEngine !");
}