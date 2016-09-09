#include "UModEditor.h"
#include "VMTConverter.h"
#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"

UMaterialInstanceConstant* VMTConverter::StartFileConversion(FString file)
{
	FScopedSlowTask Progress(3, FText::FromString("Converting VMT file '" + file + "'..."));
	Progress.MakeDialog();
	Progress.EnterProgressFrame();

	TArray<FVMTNode> Nodes;

	bool inDivision = false;
	TArray<FString> divisionStack;
	FString curDivisionName;
	FString nextDivisionName;	

	TArray<FString> FileData;
	FFileHelper::LoadANSITextFileToStrings(*file, NULL, FileData);
	for (int i = 0; i < FileData.Num(); i++) {
		FString s = FileData[i];
		s = s.Replace(TEXT(" "), TEXT(""));
		s = s.Replace(TEXT("\t"), TEXT(""));
		if (s.IsEmpty()) {
			UE_LOG(UModEditor, Log, TEXT("Escaping NULL line %i"), i + 1);
			continue;
		}
		TArray<FString> KeyVal;
		s.ParseIntoArray(KeyVal, TEXT("\""));		
		if (KeyVal.Num() == 1) {
			if (KeyVal[0] == "}") {
				inDivision = false;
				UE_LOG(UModEditor, Log, TEXT("--> End Division '%s'"), *curDivisionName);
				divisionStack.RemoveAt(0);
				if (divisionStack.Num() > 0) {					
					curDivisionName = divisionStack[0];
				}
			} else if (KeyVal[0] == "{") {
				inDivision = true;
				divisionStack.Insert(nextDivisionName, 0);				
				UE_LOG(UModEditor, Log, TEXT("--> Start Division '%s'"), *nextDivisionName);
				curDivisionName = nextDivisionName;
			} else {
				nextDivisionName = KeyVal[0];
			}
			continue;
		}
		if (KeyVal.Num() < 2) {
			UE_LOG(UModEditor, Error, TEXT("Missing key value in VMT file line %i"), i + 1);
			continue;
		}
		FString key = KeyVal[0];
		FString val = KeyVal[1];
		UE_LOG(UModEditor, Log, TEXT("Found key '%s' bound to value '%s'"), *key, *val);

		//Node creation
		FVMTNode node = FVMTNode();
		node.Name = key;
		node.Value = val;		
		FString devideStr = "";
		for (int j = divisionStack.Num() - 1; j < 1; j--) {
			if (j < 0) {
				continue;
			}
			devideStr += divisionStack[j];
		}
		node.Division = devideStr;
		Nodes.Add(node);
		//End
	}

	Progress.EnterProgressFrame();
	
	FVMTMaterial mat;	
	mat.MatType = EUModMaterialType::UMOD_OPAQUE;
	mat.IsForModel = false;
	mat.HasNormalMap = false;
	mat.AnimData = NULL;
	for (FVMTNode n : Nodes) {
		if (n.Name == "$basetexture") {
			mat.TexturePath = n.Value;
		} else if (n.Name == "$normalmap" || n.Name == "$bumpmap") {
			mat.NormalPath = n.Value;
			mat.HasNormalMap = true;
		} else if (n.Name == "$translucent") {
			if (FCString::Atoi(*n.Value) == 1) {
				mat.MatType = EUModMaterialType::UMOD_TRANSLUCENT;
			} else {
				mat.MatType = EUModMaterialType::UMOD_OPAQUE;
			}
		} else if (n.Name == "$model") {
			if (FCString::Atoi(*n.Value) == 1) {				
				mat.IsForModel = true;
			} else {
				mat.IsForModel = false;
			}
		} else if (n.Name == "$surfaceprop") {
			mat.SurfaceType = ConvertValveSurfacePropToUMod(n.Value);
		} else if (n.Name == "AnimatedTexture") {
			mat.AnimData = new FVMTAnimData();
		}
		if (mat.AnimData != NULL) {
			if (n.Name == "animatedtextureframerate") {
				mat.AnimData->VMTSourceFPS = FCString::Atoi(*n.Value);
			}
		}
	}
	if (mat.IsForModel) {
		mat.MaterialPath = VMT_MATERIAL_PATH_MDL + mat.TexturePath;
		mat.TexturePath = VMT_TEXTURE_PATH_MDL + mat.TexturePath;
		if (mat.HasNormalMap) {
			mat.NormalPath = VMT_TEXTURE_PATH_MDL + mat.NormalPath;
		}
	} else {
		mat.MaterialPath = VMT_MATERIAL_PATH_NO_MDL + mat.TexturePath;
		mat.TexturePath = VMT_TEXTURE_PATH_NO_MDL + mat.TexturePath;
		if (mat.HasNormalMap) {
			mat.NormalPath = VMT_TEXTURE_PATH_NO_MDL + mat.NormalPath;
		}
	}
	ApplySurfacePropProperties(mat.SurfaceType, mat);

	UTexture *texobj = Cast<UTexture>(StaticLoadObject(UTexture::StaticClass(), NULL, *mat.TexturePath, *mat.TexturePath));
	if (texobj == NULL) {		
		UMOD_EDIT_STANDARD_MSG("Could not load texture object for given texture path : " + mat.TexturePath);
		if (mat.AnimData != NULL) {
			delete mat.AnimData;
		}
		return NULL;
	}
	UTexture *normobj = NULL;
	if (mat.HasNormalMap) {
		normobj = Cast<UTexture>(StaticLoadObject(UTexture::StaticClass(), NULL, *mat.NormalPath, *mat.NormalPath));
		if (normobj == NULL) {
			UMOD_EDIT_STANDARD_MSG("Could not load texture object for given normal texture path : " + mat.NormalPath);
			if (mat.AnimData != NULL) {
				delete mat.AnimData;
			}
			return NULL;
		}
	}
		
	Progress.EnterProgressFrame();

	UE_LOG(UModEditor, Log, TEXT("Out material path is %s"), *mat.MaterialPath);

	UMaterialInterface *Parent = NULL;
	switch (mat.MatType) {
	case EUModMaterialType::UMOD_MASKED:
		Parent = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, UMOD_MASKED_CORE, UMOD_MASKED_CORE));
		break;
	case EUModMaterialType::UMOD_OPAQUE:
		Parent = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, UMOD_OPAQUE_CORE, UMOD_OPAQUE_CORE));
		break;
	case EUModMaterialType::UMOD_TRANSLUCENT:
		Parent = Cast<UMaterialInterface>(StaticLoadObject(UMaterialInterface::StaticClass(), NULL, UMOD_TRANSLUCENT_CORE, UMOD_TRANSLUCENT_CORE));
		break;
	}
	if (Parent == NULL) {
		UMOD_EDIT_STANDARD_MSG("Could not load MaterialCore base for object : " + mat.MaterialPath);
		if (mat.AnimData != NULL) {
			delete mat.AnimData;
		}
		return NULL;
	}
	
	UMaterialInstanceConstantFactoryNew *Factory = NewObject<UMaterialInstanceConstantFactoryNew>();
	Factory->InitialParent = Parent;
	UMaterialInstanceConstant *Instance = GenerateEmptyMaterialAtLocation(mat, Factory);
	if (Instance == NULL) {
		UMOD_EDIT_STANDARD_MSG("Failed to create asset " + mat.MaterialPath);
		if (mat.AnimData != NULL) {
			delete mat.AnimData;
		}
		return NULL;
	}
	Instance->SetParentEditorOnly(Parent);
	Instance->SetTextureParameterValueEditorOnly("Base", texobj);
	if (mat.HasNormalMap) {
		Instance->SetTextureParameterValueEditorOnly("Normal", normobj);
	}
	Instance->SetScalarParameterValueEditorOnly("Metallic", mat.MetalVar);
	Instance->SetScalarParameterValueEditorOnly("Roughness", mat.RoughnessVar);

	UPhysicalMaterial *Material = NULL;
	switch (mat.SurfaceType) {
	case EUModSurfaceType::SURFACE_METAL:
		Material = Cast<UPhysicalMaterial>(StaticLoadObject(UPhysicalMaterial::StaticClass(), NULL, METAL_PHYS_ASSET, METAL_PHYS_ASSET));
		break;
	case EUModSurfaceType::SURFACE_GLASS:
		Material = Cast<UPhysicalMaterial>(StaticLoadObject(UPhysicalMaterial::StaticClass(), NULL, GLASS_PHYS_ASSET, GLASS_PHYS_ASSET));
		break;
	case EUModSurfaceType::SURFACE_WOOD:
		Material = Cast<UPhysicalMaterial>(StaticLoadObject(UPhysicalMaterial::StaticClass(), NULL, WOOD_PHYS_ASSET, WOOD_PHYS_ASSET));
		break;
	case EUModSurfaceType::SURFACE_ROCK:
		Material = Cast<UPhysicalMaterial>(StaticLoadObject(UPhysicalMaterial::StaticClass(), NULL, ROCK_PHYS_ASSET, ROCK_PHYS_ASSET));
		break;
	}
	if (Material != NULL) {
		Instance->PhysMaterial = Material;
	} else {
		UE_LOG(UModEditor, Error, TEXT("Unable to set Physical Material for Material '%s'"), *mat.MaterialPath);
	}
	
	//TODO : Support for animations
	if (mat.AnimData != NULL) {
		delete mat.AnimData;
	}	
	return Instance;
}

UMaterialInstanceConstant* VMTConverter::GenerateEmptyMaterialAtLocation(FVMTMaterial &ref, UMaterialInstanceConstantFactoryNew* Factory)
{
	FString AssetName; //Required as UE4 seam to create assets only in a specific way
	FString AssetPathNoName; //Required as UE4 seam to create assets only in a specific way
	int pos;
	ref.MaterialPath.FindLastChar('/', pos);
	AssetName = ref.MaterialPath.Mid(pos + 1);
	AssetPathNoName = ref.MaterialPath.Mid(0, pos);
	UE_LOG(UModEditor, Log, TEXT("Asset name is '%s', asset path is '%s'"), *AssetName, *AssetPathNoName);

	IFileManager& FileManager = IFileManager::Get();
	FString AssetPathSubtractGame = ref.MaterialPath.Mid(5);
	UE_LOG(UModEditor, Log, TEXT("AssetPathSubtractGame : %s"), *AssetPathSubtractGame);
	FString AssetFile = FPaths::GameDir() + "Content" + AssetPathSubtractGame + ".uasset";
	UE_LOG(UModEditor, Log, TEXT("Operating system asset file : %s"), *AssetFile);	
	if (FileManager.FileExists(*AssetFile)) {		
		UE_LOG(UModEditor, Warning, TEXT("Found asset with same path, asking user."));
		EAppReturnType::Type Response = FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString(TEXT("An asset with the same path already exists. Do you want to overwrite it ?")), UMOD_EDIT_MSG_TITLE);
		if (Response != EAppReturnType::Yes) {
			return NULL;
		}
		//If there's already an asset loaded in RAM, unload it
		UObject *CurInstance = Cast<UObject>(StaticFindObject(UObject::StaticClass(), NULL, *ref.MaterialPath));
		if (CurInstance->IsValidLowLevel()) {
			CurInstance->ConditionalBeginDestroy();
		}
		//TODO : Delete asset file		
		if (!FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*AssetFile)) {
			UE_LOG(UModEditor, Error, TEXT("Operating system refused file deletion : %s"), *AssetFile);
			return NULL;
		}
	}
	IAssetTools &Tools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	UMaterialInstanceConstant *Instance = Cast<UMaterialInstanceConstant>(Tools.CreateAsset(AssetName, AssetPathNoName, UMaterialInstanceConstant::StaticClass(), Factory));
	return Instance;
}

EUModSurfaceType::Type VMTConverter::ConvertValveSurfacePropToUMod(FString surfaceProp)
{
	if (surfaceProp == "metal") {
		return EUModSurfaceType::SURFACE_METAL;
	} else if (surfaceProp == "wood") {
		return EUModSurfaceType::SURFACE_WOOD;
	} else if (surfaceProp == "glass") {
		return EUModSurfaceType::SURFACE_GLASS;
	} else if (surfaceProp == "rock") {
		return EUModSurfaceType::SURFACE_ROCK;
	}
	return EUModSurfaceType::SURFACE_INVALID;
}

void VMTConverter::ApplySurfacePropProperties(EUModSurfaceType::Type type, FVMTMaterial &ref)
{
	switch (type) {
	case EUModSurfaceType::SURFACE_GLASS:
		ref.RoughnessVar = 0.5;
		ref.MetalVar = 0;
		break;
	case EUModSurfaceType::SURFACE_METAL:
		ref.MetalVar = 0.5;
		ref.RoughnessVar = 1;
		break;
	case EUModSurfaceType::SURFACE_ROCK:
		ref.MetalVar = 0;
		ref.RoughnessVar = 1;
		break;
	case EUModSurfaceType::SURFACE_WOOD:
		ref.RoughnessVar = 1;
		ref.MetalVar = 0;
		break;
	case EUModSurfaceType::SURFACE_INVALID:
		UE_LOG(UModEditor, Error, TEXT("Unknown Surface Type given aborting Roughness/Metal variables import."));
		break;
	}
}

void VMTConverter::StartFolderConversion(FString folder)
{
	TArray<FString> OutFiles;
	IFileManager& FileManager = IFileManager::Get();
	FileManager.FindFiles(OutFiles, *folder);
	
	FScopedSlowTask Progress(OutFiles.Num() + 1, FText::FromString("Converting entire folder '" + folder + "'..."));
	Progress.MakeDialog();
	Progress.EnterProgressFrame();

	IContentBrowserSingleton& Browser = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get();
	TArray<UObject*> SyncObjs;

	for (FString file : OutFiles) {		
		UMaterialInstanceConstant *File = StartFileConversion(FString(folder + "/" + file));
		if (File != NULL) {
			SyncObjs.Add(File);
		}

		Progress.EnterProgressFrame();
	}

	Browser.SyncBrowserToAssets(SyncObjs);
}