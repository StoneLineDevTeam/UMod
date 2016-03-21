// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "Render2D.generated.h"

struct FVertex {
	int X;
	int Y;
	int U;
	int V;

	FVertex(int x, int y, int u, int v) {
		X = x;
		Y = y;
		U = u;
		V = v;
	}
};

#define TEXT_ALIGN_CENTER 1
#define TEXT_ALIGN_LEFT 0
#define TEXT_ALIGN_RIGHT 2

/**
 * 
 */
UCLASS()
class UMOD_API URender2D : public UObject
{
	GENERATED_BODY()
	
public:
	static void PushMatrix(FMatrix mat);
	static void PopMatrix();

	static void SetContext(UCanvas *c);
	static void ExitContext();

	//Set draw color for all upcoming draw operations
	static void SetColor(FColor col);
	//Set draw texture for all compatible upcoming draw operations
	static void SetTexture(uint32 id);
	static void ResetTexture();
	//Set text font
	static void SetFont(uint32 id);
	//Loads font from path
	static uint32 LoadFont(FString path);
	//Unloads font
	static void UnloadFont(uint32 id);
	//Loads texture from path
	static uint32 LoadTexture(FString path);
	//Unloads texture
	static void UnloadTexture(uint32 id);
	//Draws a rectangle
	static void DrawRect(int x, int y, int w, int h);
	//Draw an outline rectangle
	static void DrawOutlineRect(int x, int y, int w, int h, int stroke);
	//Draw polygon with vertex array (NOTE : need special code...)
	static void DrawPoly(TArray<FVertex> vertices);
	//Draw a circle with given rayon (NOTE : need special code...)
	static void DrawCircle(int x, int y, int r);
	//Draw a rounded rectangle (NOTE : need special code...)
	static void DrawRoundedRect(int x, int y, int w, int h);
	//Set the scissoring rectangle (NOTE : need special code...)
	static void SetScissorRect(int x, int y, int w, int h);
	//Draws a string
	static void DrawText(FString str, int x, int y, uint8 align);
	//Return the size of a text
	static void GetTextSize(FString str, float& w, float& h);
	//Set the font scale
	static void SetFontScale(float sx, float sy);
	//Draw a line starting from (x, y) and ending to (x1, y1) using stroke
	static void DrawLine(float x, float y, float x1, float y1, float stroke);
private:
	static void FindNextTextureMapSlot();
	static void FindNextFontMapSlot();
};
