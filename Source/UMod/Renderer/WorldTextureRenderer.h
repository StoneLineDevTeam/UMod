#pragma once
#include "UMod.h"
#include "WorldTextureRenderer.generated.h"

UCLASS()
class UWorldTextureRenderer : public UPrimitiveComponent {
	GENERATED_BODY()

public:
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual FBoxSphereBounds CalcBounds(const FTransform & LocalToWorld) const override;
	virtual UBodySetup* GetBodySetup() override;
	virtual FCollisionShape GetCollisionShape(float Inflation) const override;

	bool HasTransparency();
	UMaterialInstanceDynamic *Get3DSpaceMaterial();
private:
	bool EnabledTransparency;
	UMaterialInstanceDynamic *Material;
};

UCLASS()
class UScreenRenderTarget : public UCanvasRenderTarget2D {
	GENERATED_BODY()

public:
	void FireRender(UCanvas *Canvas, int32 W, int32 H);
	void Init(uint8 ID);
private:
	uint8 GivenID;
};