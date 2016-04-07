#include "UMod.h"
#include "WorldTextureRenderer.h"
#include "DynamicMeshBuilder.h"
#include "Render2D.h"
#include "UModGameInstance.h"


class FWorldTextureRenderer : public FPrimitiveSceneProxy {

public:
	FWorldTextureRenderer(UWorldTextureRenderer *Component) : FPrimitiveSceneProxy(Component), TransparencyEnabled(Component->HasTransparency()), Material(Component->Get3DSpaceMaterial())
	{
		bWillEverBeLit = false;
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const
	{
		FMaterialRenderProxy* MaterialProxy = Material->GetRenderProxy(IsSelected()); //Get the lower level Material representation to apply on the built mesh (for use in MeshBuilder.GetMesh())
		const FMatrix& LocalToWorld = GetLocalToWorld(); //LocalToWorld matrix for use in MeshBuilder.GetMesh()

		for (int i = 0; i < Views.Num(); i++) {
			const FSceneView *View = Views[i];
			FDynamicMeshBuilder MeshBuilder;

			//TODO : Draw a screen mesh using the MeshBuilder
		}		
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View)
	{
		FPrimitiveViewRelevance Relevance;
		Relevance.bDrawRelevance = IsShown(View);
		Relevance.bDynamicRelevance = true;
		Relevance.bEditorPrimitiveRelevance = false;
		Relevance.bHasSimpleLights = false;
		Relevance.bShadowRelevance = IsShadowCast(View);
		Relevance.bOpaqueRelevance = !TransparencyEnabled;
		Relevance.bSeparateTranslucencyRelevance = TransparencyEnabled;
		Relevance.bMaskedRelevance = false;
		return Relevance;
	}

	virtual uint32 GetMemoryFootprint(void) const { return(sizeof(*this) + GetAllocatedSize()); }

private:
	bool TransparencyEnabled;
	UMaterialInstanceDynamic *Material;
};


FPrimitiveSceneProxy* UWorldTextureRenderer::CreateSceneProxy()
{
	if (Material != NULL) {
		return new FWorldTextureRenderer(this);
	}
	return NULL;
}

FBoxSphereBounds UWorldTextureRenderer::CalcBounds(const FTransform & LocalToWorld) const
{
	return FBoxSphereBounds();
}

UBodySetup* UWorldTextureRenderer::GetBodySetup()
{
	return NULL;
}

FCollisionShape UWorldTextureRenderer::GetCollisionShape(float Inflation) const
{
	return FCollisionShape::MakeBox(FVector(0, 0, 0));
}	

UMaterialInstanceDynamic *UWorldTextureRenderer::Get3DSpaceMaterial()
{
	return Material;
}

bool UWorldTextureRenderer::HasTransparency()
{
	return EnabledTransparency;
}


//UCanvasRenderTarget2D implementation
void UScreenRenderTarget::FireRender(UCanvas *Canvas, int32 W, int32 H)
{
	URender2D::SetContext(Canvas);
	UUModGameInstance *UMod = Cast<UUModGameInstance>(GetWorld()->GetGameInstance());
	//Call Lua function RenderScreen with 3 params ScreenID, Width, Height
	UMod->Lua->RunScriptFunctionThreeParam<int, int, int>(ETableType::GAMEMODE, 0, "RenderScreen", FLuaParam<int>((int)GivenID), FLuaParam<int>(W), FLuaParam<int>(H));
	URender2D::ExitContext();
}

void UScreenRenderTarget::Init(uint8 ID)
{
	OnCanvasRenderTargetUpdate.AddDynamic(this, &UScreenRenderTarget::FireRender);
	GivenID = ID;
}