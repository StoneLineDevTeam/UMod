#include "UMod.h"
#include "Render3D.h"
#include "WorldTextureRenderer.h"

//Variables
static TArray<UScreenRenderTarget*> WorldScreens;
static uint8 CurUsed3D2DTarget;
static UWorld *W;

uint8 URender3D::Create3D2DTarget(uint32 Width, uint32 Height)
{
	UScreenRenderTarget *Target = Cast<UScreenRenderTarget>(UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(W, UScreenRenderTarget::StaticClass(), Width, Height));
	WorldScreens.Add(Target);
	uint8 ID = (uint8)(WorldScreens.Num() - 1);
	Target->Init(ID);
	return ID;
}

void URender3D::Render3D2DTarget(uint8 id)
{
	if (WorldScreens.Num() < id) { return; }
	if (WorldScreens[id] == NULL) { return; }
	UScreenRenderTarget *Target = WorldScreens[id];
	Target->UpdateResource();
}

void URender3D::SetWorld(UWorld *World)
{
	W = World;
}