--Here is the code to add a dropdown menu to the editor.
--NOTE : Needs creation of editor module !
--FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
--LevelEditorModule.GetMenuExtensibilityManager()->AddExtender( Extender );

--PUT CONFIG INSIDE Saved/Config otherwise fucking UE4 will understand that you want to compile this file in the pak, which results in unmodifiable config files !