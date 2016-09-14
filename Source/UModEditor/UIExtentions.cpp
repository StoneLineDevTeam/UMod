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
void UIExtentions::PrepareLuaDocs()
{
	FString LuaSrc = FPaths::GameDir() + "/Source/UMod/Lua/";
	FString LuaSrcParseFile = FPaths::GameDir() + "/LuaDocs/ParseList.txt";
	FString LuaDocsInterPath = FPaths::GameDir() + "/LuaDocs/Intermediate/";

	IFileManager& FileManager = IFileManager::Get();
	if (!FileManager.FileExists(*LuaSrcParseFile)) {
		UMOD_EDIT_STANDARD_MSG("The ParseList file does not exist, aborting Lua docs prepare...");
		return;
	}

	if (FileManager.DirectoryExists(*LuaDocsInterPath)) {
		UMOD_EDIT_STANDARD_MSG("Please delete the Intemediate folder inside LuaDocs before preparing again Lua docs !");
		return;
	}

	if (!FileManager.FileExists(*LuaDocsInterPath)) {
		FileManager.MakeDirectory(*LuaDocsInterPath);
		FileManager.MakeDirectory(*(LuaDocsInterPath + "libs/"));
		FileManager.MakeDirectory(*(LuaDocsInterPath + "basetypes/"));
		FileManager.MakeDirectory(*(LuaDocsInterPath + "types/"));
		FileManager.MakeDirectory(*(LuaDocsInterPath + "others/"));
	}

	FScopedSlowTask Progress(3, FText::FromString("Reading ParseList.txt..."));
	Progress.MakeDialog();
	Progress.EnterProgressFrame();

	TArray<FString> FileData;
	FFileHelper::LoadANSITextFileToStrings(*LuaSrcParseFile, NULL, FileData);

	Progress.EnterProgressFrame();

	FScopedSlowTask ProgressSrc(FileData.Num() + 1, FText::FromString("Parsing source files..."));
	ProgressSrc.MakeDialog();
	ProgressSrc.EnterProgressFrame();

	for (FString file : FileData) {
		TArray<FString> Code;		
		FFileHelper::LoadANSITextFileToStrings(*(LuaSrc + file), NULL, Code);
		
		FString FileContent = "";

		TArray<FString> args;
		TArray<FString> rets;

		FString FuncNameReplace = "";
		FString OutTxtPath = LuaDocsInterPath;
		ELuaDocType type;
		if (file.StartsWith("LuaLib", ESearchCase::CaseSensitive)) {
			FString fileName = file.Mid(6, file.Len() - 6 - 4); //Must be FileName.cpp
			FuncNameReplace = fileName;
			OutTxtPath += "libs/" + fileName + ".txt";
			type = LUA_LIBRARY;
		} else if (file.StartsWith("Interface/", ESearchCase::CaseSensitive)) {
			FString fileName = file.Mid(13);
			fileName = fileName.Mid(0, fileName.Len() - 2); //Must be FileName.h
			FuncNameReplace = fileName;
			OutTxtPath += "basetypes/" + fileName + ".txt";
			type = LUA_BASETYPE;
		} else if (file.StartsWith("Lua", ESearchCase::CaseSensitive)) {
			FString fileName = file.Mid(3, file.Len() - 3 - 4); //Must be FileName.cpp
			FuncNameReplace = fileName;
			OutTxtPath += "types/" + fileName + ".txt";
			type = LUA_TYPE;
		} else {
			FString fileName = file.Mid(0, file.Len() - 4); //Must be FileName.cpp
			FuncNameReplace = fileName;
			OutTxtPath += "others/" + fileName + ".txt";
			type = LUA_OTHER;
		}

		FileContent += "#Warning this file format uses double quotes characters (\") to allow spaces inside descriptions.\n";
		switch (type) {
		case LUA_LIBRARY:
			FileContent += "#Use this file to configure descriptions for library '" + FuncNameReplace + "'.\n";
			break;
		case LUA_TYPE:
			FileContent += "#Use this file to configure descriptions for type '" + FuncNameReplace + "'.\n";
			break;
		case LUA_BASETYPE:
			FileContent += "#Use this file to configure descriptions for base-type '" + FuncNameReplace + "'.\n";
			break;
		case LUA_OTHER:
			FileContent += "#Use this file to configure descriptions for other doc '" + FuncNameReplace + "'.\n";
			break;
		}

		FileContent += "\n";

		int bracketStack = -1;
		bool InLuaFunc = false;		
		for (FString Line : Code) {
			Line = Line.Replace(TEXT(" "), TEXT(""));
			Line = Line.Replace(TEXT("\t"), TEXT(""));
			if (Line.StartsWith("DECLARE_LUA_FUNC", ESearchCase::CaseSensitive)) {
				InLuaFunc = true;
				bool HasArgs = true;
				int start = 0;
				int end = 0;
				bool b = Line.FindChar('(', start);
				bool b1 = Line.FindChar(',', end);
				if (!b1) {
					HasArgs = false;
					b1 = Line.FindChar(')', end);
				}
				if (!b || !b1) {
					continue;
				}				
				SUBSTR(FuncName, Line, start + 1, end);
				if (FuncName.Contains("_", ESearchCase::CaseSensitive)) {
					FuncName = FuncName.Replace(*(FuncNameReplace + "_"), TEXT(""), ESearchCase::CaseSensitive);
				} else {
					FuncName = FuncName.Replace(*FuncNameReplace, TEXT(""), ESearchCase::CaseSensitive);
				}
				//Let's parse the rest of the macro here
				if (HasArgs) {
					SUBSTR(VA_ARGS, Line, end, Line.Len() - 1);
					VA_ARGS.ParseIntoArray(args, TEXT(","));
				}
				bracketStack = 0;
				FileContent += FuncName + " = {\n"; //The '=' will be parsed by the doc generator in order to know function description, function args and function returns
			} else if (Line == "}" || Line.Contains("{")) {
				if (bracketStack < 0 || !InLuaFunc) {
					continue;
				}
				if (Line == "}" && bracketStack > 0) {
					bracketStack--;
				} else if (Line.Contains("{")) {
					bracketStack++;
				}
				if (bracketStack == 0) {
					InLuaFunc = false;
					FileContent += "\tARGUMENTS\n";
					for (int i = 0; i < args.Num(); i++) {
						FileContent += "\t\t" + args[i] + " : \n";
					}
					FileContent += "\tEND\n";
					FileContent += "\tRETURNS\n";
					for (int i = 0; i < rets.Num(); i++) {
						FileContent += "\t\t" + rets[i] + ":\n";
					}
					FileContent += "\tEND\n";
					FileContent += "}\n";

					args.Empty();
					rets.Empty();
				}
			} else {
				//I want to go further I know, but I like to speed up things....
				if (Line.StartsWith("Lua.Push")) { //We have a return to our function
					int i = 0;
					if (Line.FindChar('(', i)) {
						SUBSTR(retType, Line, 8, i);
						if (retType != "Value") {
							rets.Add(retType);
						}
					}
				}
			}
		}		
		FFileHelper::SaveStringToFile(FileContent, *OutTxtPath);

		ProgressSrc.EnterProgressFrame();
	}

	Progress.EnterProgressFrame();
}
void UIExtentions::GenerateLuaDocs()
{

}
void UIExtentions::OpenLauncher()
{
	if (!FPlatformProcess::IsApplicationRunning(TEXT("EpicGamesLauncher")) && !FPlatformProcess::IsApplicationRunning(TEXT("EpicGamesLauncher-Mac-Shipping"))) {
		FDesktopPlatformModule::Get()->OpenLauncher(FOpenLauncherOptions(false, TEXT("")));
	}
}
void UIExtentions::OpenMessageLog()
{
	FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
	MessageLogModule.EnableMessageLogDisplay(true);
	MessageLogModule.OpenMessageLog("UModEditor");
}
bool IsUModEntity(UClass *cl) {
	return cl->GetName().StartsWith("Entity");
}
void UIExtentions::MapCheckErrors()
{
	bool HasSpawn = false;
	bool HasSkybox = false;
	UWorld *WorldCtx = GEngine->GetEditorWorldContext().World();
	LogEditorMessage(LEVEL_INFO, "--> Running Map Check <--");
	int CntBase = 0;
	int CntUE4 = 0;
	for (TObjectIterator<AActor> Itr; Itr; ++Itr) {
		if (*Itr != NULL && Itr->GetWorld() == WorldCtx) {
			LogEditorMessage(LEVEL_INFO, "Checking Actor '" + Itr->GetName() + "' of type '" + Itr->GetClass()->GetName() + "'...");
			if (IsUModEntity(Itr->GetClass())) {
				CntBase++;
			} else {
				FString act = Itr->GetClass()->GetName();
				if (act == "PlayerStart") {
					HasSpawn = true;
				} else if (act == "BP_Sky_Sphere_C") {
					HasSkybox = true;
				} else if (act == "StaticMeshActor") {
					TArray<UStaticMeshComponent*> comps;
					Itr->GetComponents<UStaticMeshComponent>(comps);
					bool del = false;
					for (int32 i = 0; i < comps.Num(); i++) {
						UStaticMeshComponent *c = comps[i];
						if (c->IsSimulatingPhysics()) {
							del = true;
							break;
						}
					}
					if (del) {
						LogEditorMessage(LEVEL_ERROR, "StaticMeshComponent with physics detected ! That is not allowed, please use EntityPhysicsProp instead.");
					}
				}

				CntUE4++;
			}			
		}
	}
	LogEditorMessage(LEVEL_INFO, "--> Map specs for '" + WorldCtx->GetName() + "' <--");
	if (CntBase > 1) {
		LogEditorMessage(LEVEL_INFO, FString::FromInt(CntBase + CntUE4) + " total checked actors (" + FString::FromInt(CntBase) + " UMod entities, " + FString::FromInt(CntUE4) + " UE4 actor(s))");
	} else {
		LogEditorMessage(LEVEL_INFO, FString::FromInt(CntBase + CntUE4) + " total checked actors (" + FString::FromInt(CntBase) + " UMod entity, " + FString::FromInt(CntUE4) + " UE4 actor(s))");
	}
	if (!HasSpawn) {
		LogEditorMessage(LEVEL_ERROR, "This map has no spawn location defined, please add a PlayerStart actor !");
	}
	if (!HasSkybox) {
		LogEditorMessage(LEVEL_WARNING, "This map has no Skybox, this may cause rendering issues...");
	}
	LogEditorMessage(LEVEL_INFO, "--> End section <--");
}
void UIExtentions::MapSetupSkybox()
{
	
}
void UIExtentions::LuaMenu(FMenuBuilder &MenuBuilder)
{
	MenuBuilder.AddMenuEntry(FText::FromString("Prepare Lua docs"), FText::FromString("Prepare Lua docs (Requires Source Code)"), FSlateIcon::FSlateIcon(), FUIAction(FExecuteAction::CreateRaw(this, &UIExtentions::PrepareLuaDocs)));
	MenuBuilder.AddMenuEntry(FText::FromString("Generate Lua docs"), FText::FromString("Generates Lua docs (Requires Source Code)"), FSlateIcon::FSlateIcon(), FUIAction(FExecuteAction::CreateRaw(this, &UIExtentions::GenerateLuaDocs)));
}
void UIExtentions::MapToolsMenu(FMenuBuilder &MenuBuilder)
{
	MenuBuilder.AddMenuEntry(FText::FromString("Check Errors"), FText::FromString("Check mapping errors"), FSlateIcon::FSlateIcon(), FUIAction(FExecuteAction::CreateRaw(this, &UIExtentions::MapCheckErrors)));
	MenuBuilder.AddMenuEntry(FText::FromString("Setup Skybox"), FText::FromString("Sets up a map with a default skybox and a light"), FSlateIcon::FSlateIcon(), FUIAction(FExecuteAction::CreateRaw(this, &UIExtentions::MapSetupSkybox)));
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
	MenuBuilder.AddSubMenu(FText::FromString("VMT Converter"), FText::FromString("Convert VMT file representation into UMod Lua ready materials"), FNewMenuDelegate::CreateRaw(this, &UIExtentions::ConvertMaterialMenu));
	MenuBuilder.AddSubMenu(FText::FromString("Lua"), FText::FromString("Lua doc generator commands"), FNewMenuDelegate::CreateRaw(this, &UIExtentions::LuaMenu));
	MenuBuilder.AddSubMenu(FText::FromString("MapTools"), FText::FromString("UMod map tools"), FNewMenuDelegate::CreateRaw(this, &UIExtentions::MapToolsMenu));
	MenuBuilder.AddMenuEntry(FText::FromString("Message Log"), FText::FromString("Opens UE4's message log in UModEditor category"), FSlateIcon::FSlateIcon(), FUIAction(FExecuteAction::CreateRaw(this, &UIExtentions::OpenMessageLog)));
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