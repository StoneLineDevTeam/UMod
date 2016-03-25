#pragma once

class FCanvasScissorRectItem : public FCanvasItem
{
public:
	FCanvasScissorRectItem(int x, int y, int w, int h);
	FCanvasScissorRectItem();

	void Draw(FCanvas* InCanvas);
private:
	FIntRect Rect;
};