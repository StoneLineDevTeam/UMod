// Fill out your copyright notice in the Description page of Project Settings.

#include "UMod.h"
#include "Render2D.h"

//Statics here as linker is unable to link correctly with static vars inside H file
static FVector2D FontScale;

static FColor CurColor;
static uint32 CurFont;
static uint32 CurTexture;
static bool NoTex;
static UCanvas *Context;
static TMap<uint32, UTexture2D*> Textures;
static TMap<uint32, UFont*> Fonts;

static uint32 EmptyTextureSlot;
static uint32 EmptyFontSlot;
//End

void URender2D::FindNextTextureMapSlot()
{
	while (Textures.Find(EmptyTextureSlot)) {
		EmptyTextureSlot++;
	}
}
void URender2D::FindNextFontMapSlot()
{
	while (Fonts.Find(EmptyFontSlot)) {
		EmptyFontSlot++;
	}
}

void URender2D::PushMatrix(FMatrix mat)
{
	Context->Canvas->PushAbsoluteTransform(mat);
}

void URender2D::PopMatrix()
{
	Context->Canvas->PopTransform();
}

void URender2D::SetContext(UCanvas *c)
{
	Context = c;
}

void URender2D::ExitContext()
{
	Context = NULL;
	NoTex = false;
}

//Set draw color for all upcoming draw operations
void URender2D::SetColor(FColor col)
{
	CurColor = col;
}

//Set draw texture for all compatible upcoming draw operations
void URender2D::SetTexture(uint32 id)
{
	CurTexture = id;
	NoTex = false;
}

void URender2D::ResetTexture()
{
	NoTex = true;
}

//Set text font
void URender2D::SetFont(uint32 id)
{
	CurFont = id;
}

//Loads font from path
uint32 URender2D::LoadFont(FString path)
{
	uint32 id = EmptyFontSlot;
	UFont* fnt = LoadObjFromPath<UFont>(*("/Game/Fonts/" + path));
	Fonts.Add(id, fnt);
	FindNextFontMapSlot();
	return id;
}

//Unloads font
void URender2D::UnloadFont(uint32 id)
{
	EmptyFontSlot = id;
	Fonts.Remove(EmptyFontSlot);
}

//Loads texture from path
uint32 URender2D::LoadTexture(FString path)
{
	uint32 id = EmptyTextureSlot;
	UTexture2D* tex = LoadObjFromPath<UTexture2D>(*("/Game/" + path));
	Textures.Add(id, tex);
	FindNextTextureMapSlot();
	return id;
}

//Unloads texture
void URender2D::UnloadTexture(uint32 id)
{
	EmptyTextureSlot = id;
	Textures.Remove(EmptyTextureSlot);
}

//Draws a rectangle
void URender2D::DrawRect(int x, int y, int w, int h)
{
	FCanvasTileItem Rectangle(FVector2D(x, y), FVector2D(w, h), FLinearColor(CurColor));
	if (!NoTex && Textures[CurTexture] != NULL) {
		FTexture* tex = Textures[CurTexture]->Resource;
		Rectangle = FCanvasTileItem(FVector2D(x, y), tex, FVector2D(w, h), FLinearColor(CurColor));
	}
	
	Rectangle.BlendMode = SE_BLEND_Translucent;
	Context->DrawItem(Rectangle);	
}

//Draw an outline rectangle
void URender2D::DrawOutlineRect(int x, int y, int w, int h, int stroke)
{
	DrawRect(x, y, x + stroke, h);
	DrawRect((x + w) - stroke, y, stroke, h);
	DrawRect(x, y, w, stroke);
	DrawRect(x, (y + h) - stroke, w, stroke);
}

//Draw polygon with vertex array (NOTE : need special code...)
void URender2D::DrawPoly(TArray<FVertex> vertices)
{

}

//Draw a circle with given rayon (NOTE : need special code...)
void URender2D::DrawCircle(int x, int y, int r)
{

}

//Draw a rounded rectangle (NOTE : need special code...)
void URender2D::DrawRoundedRect(int x, int y, int w, int h)
{

}

//Set the scissoring rectangle (NOTE : need special code...)
void URender2D::SetScissorRect(int x, int y, int w, int h)
{

}

//Draws a string
void URender2D::DrawText(FString str, int x, int y, uint8 align)
{
	UFont *fnt = Fonts[CurFont];
	if (fnt == NULL) {
		return;
	}

	float TW, TH;
	GetTextSize(str, TW, TH);

	float TextX = 0;
	float TextY = y;

	switch (align)
	{
	case 0:
		TextX = x;
		break;
	case 1:
		TextX = x - TW / 2;
		break;
	case 2:
		TextX = x - TW;
		break;
	}

	FCanvasTextItem Text(FVector2D(TextX, TextY), FText::FromString(str), fnt, CurColor);
	Text.Scale = FontScale;
	Context->DrawItem(Text);
}

void URender2D::SetFontScale(float sx, float sy)
{
	FontScale = FVector2D(sx, sy);
}

void URender2D::GetTextSize(FString str, float& w, float& h)
{
	UFont *fnt = Fonts[CurFont];
	if (fnt == NULL) {
		return;
	}

	Context->TextSize(fnt, str, w, h, FontScale.X, FontScale.Y);
}

void URender2D::DrawLine(float x, float y, float x1, float y1, float stroke)
{
	Context->K2_DrawLine(FVector2D(x, y), FVector2D(x1, y1), stroke, FLinearColor(CurColor));
}
