#pragma once
#include "UModEditor.h"

#define VMT_TEXTURE_PATH_NO_MDL "/Game/UMod/Materials/Textures/"
#define VMT_TEXTURE_PATH_MDL "/Game/UMod/Materials/Textures/Models/"
#define VMT_MATERIAL_PATH_NO_MDL "/Game/UMod/Materials/"
#define VMT_MATERIAL_PATH_MDL "/Game/UMod/Materials/Models/"
#define UMOD_TRANSLUCENT_CORE TEXT("/Game/UMod/Materials/MaterialCore/UModTranslucent")
#define UMOD_OPAQUE_CORE TEXT("/Game/UMod/Materials/MaterialCore/UModOpaque")
#define UMOD_MASKED_CORE TEXT("/Game/UMod/Materials/MaterialCore/UModMasked")

#define METAL_PHYS_ASSET TEXT("/Game/UMod/Materials/MaterialCore/Metal")
#define GLASS_PHYS_ASSET TEXT("/Game/UMod/Materials/MaterialCore/Glass")
#define WOOD_PHYS_ASSET TEXT("/Game/UMod/Materials/MaterialCore/Wood")
#define ROCK_PHYS_ASSET TEXT("/Game/UMod/Materials/MaterialCore/Rock")

namespace EUModSurfaceType {
	enum Type {
		SURFACE_METAL,
		SURFACE_GLASS,
		SURFACE_WOOD,
		SURFACE_ROCK,
		SURFACE_INVALID
	};
}

namespace EAnimationType {
	enum Type {
		ANIM_LEGACY,
		ANIM_PANNER
	};
}

namespace EUModMaterialType {
	enum Type {
		UMOD_OPAQUE,
		UMOD_TRANSLUCENT,
		UMOD_MASKED
	};
}

struct FVMTNode {
	FString Name;
	FString Value;
	FString Division;
};

struct FVMTAnimData {
	int VMTSourceFPS;
	int StartFrame;
	int TickElapsedFrames;
	EAnimationType::Type type;
	float PannerSpeedU;
	float PannerSpeedV;
};

struct FVMTMaterial {
	FString TexturePath;
	FString NormalPath;
	FString MaterialPath;
	float MetalVar; //@TODO
	float RoughnessVar; //@TODO
	EUModSurfaceType::Type SurfaceType; //@TODO
	FVMTAnimData *AnimData; //@TODO
	EUModMaterialType::Type MatType;
	bool IsForModel;
	bool HasNormalMap;
};

class VMTConverter {
public:
	static void StartFolderConversion(FString folder);
	static UMaterialInstanceConstant* StartFileConversion(FString file);
private:
	static EUModSurfaceType::Type ConvertValveSurfacePropToUMod(FString surfaceProp);
	static void ApplySurfacePropProperties(EUModSurfaceType::Type type, FVMTMaterial &ref);
	static UMaterialInstanceConstant* GenerateEmptyMaterialAtLocation(FVMTMaterial &ref, UMaterialInstanceConstantFactoryNew* Factory);
};