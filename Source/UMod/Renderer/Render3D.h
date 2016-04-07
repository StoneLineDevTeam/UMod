#pragma once
#include "UMod.h"
#include "Object.h"
#include "Render3D.generated.h"

UCLASS()
class UMOD_API URender3D : public UObject {
	GENERATED_BODY()

public:
	static uint8 Create3D2DTarget(uint32 Width, uint32 Height); //For the moment limit number of 3D2D screens to 256
	static void Render3D2DTarget(uint8 id); //Renders the specified 3D2D screen target (will call automatically the GM:RenderScreen function with the Context already set)

	//Will be called internally by the game
	static void SetWorld(UWorld *World);
};