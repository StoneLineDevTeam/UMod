#include "UMod.h"
#include "LuaLibSurface.h"
#include "LuaEngine.h"
#include "Renderer/Render2D.h"

/*Base surface.* library*/
static bool Check2DRenderingContext(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	if (!URender2D::CheckContext()) {
		Lua.ThrowError("Not in a 2D rendering context.");
		return false;
	}
	return true;
}
static int LoadFont(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	uint32 i = URender2D::LoadFont(Lua.CheckString(-3), Lua.CheckInt(-2), FName(*Lua.CheckString(-1)));
	Lua.PushInt(i);
	return 1;
}
static int LoadTexture(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	uint32 i = URender2D::LoadTexture(Lua.CheckString(-1));
	Lua.PushInt(i);
	return 1;
}
static int UnloadFont(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	URender2D::UnloadFont(Lua.CheckInt(-1));
	return 0;
}
static int UnloadTexture(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	URender2D::UnloadTexture(Lua.CheckInt(-1));
	return 0;
}
static int SetColor(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	if (!Check2DRenderingContext(L)) { return 0; }
	URender2D::SetColor(Lua.CheckColor(-1));
	return 0;
}
static int SetTexture(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	if (!Check2DRenderingContext(L)) { return 0; }
	URender2D::SetTexture(Lua.CheckInt(-1));
	return 0;
}
static int ResetTexture(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	if (!Check2DRenderingContext(L)) { return 0; }
	URender2D::ResetTexture();
	return 0;
}
static int SetFont(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	if (!Check2DRenderingContext(L)) { return 0; }
	URender2D::SetFont(Lua.CheckInt(-1));
	return 0;
}
static int DrawRect(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	if (!Check2DRenderingContext(L)) { return 0; }
	URender2D::DrawRect(Lua.CheckFloat(-4), Lua.CheckFloat(-3), Lua.CheckFloat(-2), Lua.CheckFloat(-1));
	return 0;
}
static int DrawOutlineRect(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	if (!Check2DRenderingContext(L)) { return 0; }
	URender2D::DrawOutlineRect(Lua.CheckFloat(-5), Lua.CheckFloat(-4), Lua.CheckFloat(-3), Lua.CheckFloat(-2), Lua.CheckFloat(-1));
	return 0;
}
static int SetScissorRect(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	if (!Check2DRenderingContext(L)) { return 0; }
	URender2D::SetScissorRect(Lua.CheckInt(-4), Lua.CheckInt(-3), Lua.CheckInt(-2), Lua.CheckInt(-1));
	return 0;
}
static int DrawText(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	if (!Check2DRenderingContext(L)) { return 0; }
	URender2D::DrawText(Lua.CheckString(-4), Lua.CheckFloat(-3), Lua.CheckFloat(-2), Lua.CheckInt(-1));
	return 0;
}
static int GetTextSize(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	if (!Check2DRenderingContext(L)) { return 0; }
	float w;
	float h;
	URender2D::GetTextSize(Lua.CheckString(-1), w, h);
	Lua.PushFloat(w);
	Lua.PushFloat(h);
	return 2;
}
static int SetFontScale(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	if (!Check2DRenderingContext(L)) { return 0; }
	URender2D::SetFontScale(Lua.CheckFloat(-2), Lua.CheckFloat(-1));
	return 0;
}
static int DrawLine(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	if (!Check2DRenderingContext(L)) { return 0; }
	URender2D::DrawLine(Lua.CheckFloat(-5), Lua.CheckFloat(-4), Lua.CheckFloat(-3), Lua.CheckFloat(-2), Lua.CheckFloat(-1));
	return 0;
}
static int DrawCircle(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	if (!Check2DRenderingContext(L)) { return 0; }
	URender2D::DrawCircle(Lua.CheckFloat(-3), Lua.CheckFloat(-2), Lua.CheckInt(-1));
	return 0;
}
static int DrawRoundedRect(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	if (!Check2DRenderingContext(L)) { return 0; }
	URender2D::DrawRoundedRect(Lua.CheckFloat(-5), Lua.CheckFloat(-4), Lua.CheckFloat(-3), Lua.CheckFloat(-2), Lua.CheckInt(-1));
	return 0;
}
//FUTURE METHODS : NEEDS UE4 C++ TRICKS
static int DrawPoly(lua_State *L) { //Not sure that the function works (lua side as in all cases URender2D has empty function body for DrawPoly...
	LuaInterface Lua = LuaInterface::Get(L);
	if (!Check2DRenderingContext(L)) { return 0; }
	TArray<FVertex> Vertices;
	while (Lua.Next(-1) != 0) {
		Lua.PushString("X");
		Lua.GetTable(-2);
		float x = Lua.CheckFloat(-1);
		Lua.PushString("Y");
		Lua.GetTable(-2);
		float y = Lua.CheckFloat(-1);
		Lua.PushString("U");
		Lua.GetTable(-2);
		float u = Lua.CheckFloat(-1);
		Lua.PushString("V");
		Lua.GetTable(-2);
		float v = Lua.CheckFloat(-1);
		Lua.Pop(-1);
		Vertices.Add(FVertex(x, y, u, v));
	}
	URender2D::DrawPoly(Vertices);
	return 0;
}
/*End*/

void LuaLibSurface::RegisterSurfaceLib(LuaEngine *Lua)
{
	Lua->BeginLibReg("surface");
	Lua->AddLibFunction("LoadFont", LoadFont);
	Lua->AddLibFunction("LoadTexture", LoadTexture);
	Lua->AddLibFunction("UnloadFont", UnloadFont);
	Lua->AddLibFunction("UnloadTexture", UnloadTexture);
	Lua->AddLibFunction("SetColor", SetColor);
	Lua->AddLibFunction("SetTexture", SetTexture);
	Lua->AddLibFunction("ResetTexture", ResetTexture);
	Lua->AddLibFunction("SetFont", SetFont);
	Lua->AddLibFunction("DrawRect", DrawRect);
	Lua->AddLibFunction("DrawOutlineRect", DrawOutlineRect);
	Lua->AddLibFunction("SetScissorRect", SetScissorRect);
	Lua->AddLibFunction("DrawText", DrawText);
	Lua->AddLibFunction("GetTextSize", GetTextSize);
	Lua->AddLibFunction("SetFontScale", SetFontScale);
	Lua->AddLibFunction("DrawLine", DrawLine);
	Lua->AddLibFunction("DrawCircle", DrawCircle);
	Lua->AddLibFunction("DrawRoundedRect", DrawRoundedRect);
	Lua->AddLibFunction("DrawPoly", DrawPoly);
	Lua->CreateLibrary();

	Lua->BeginLibReg("DrawEnums");
	Lua->AddLibConstant("TEXT_ALIGN_CENTER", 1);
	Lua->AddLibConstant("TEXT_ALIGN_LEFT", 0);
	Lua->AddLibConstant("TEXT_ALIGN_RIGHT", 2);
	Lua->CreateLibrary();
}