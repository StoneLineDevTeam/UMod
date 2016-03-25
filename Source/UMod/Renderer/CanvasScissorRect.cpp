#include "UMod.h"
#include "CanvasScissorRect.h"

FCanvasScissorRectItem::FCanvasScissorRectItem(int x, int y, int w, int h):FCanvasItem(FVector2D::ZeroVector)
{
	Rect = FIntRect(x, y, x + w, y + h);
}

FCanvasScissorRectItem::FCanvasScissorRectItem():FCanvasScissorRectItem(0, 0, 0, 0)
{
}

void FCanvasScissorRectItem::Draw(FCanvas* InCanvas)
{
	ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(CanvasScissorRect, FIntRect, RectInt, Rect, {
		RHICmdList.SetScissorRect(true, RectInt.Min.X, RectInt.Min.Y, RectInt.Max.X, RectInt.Max.Y);
	});
}