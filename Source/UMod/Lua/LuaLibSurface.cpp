#include "UMod.h"
#include "LuaLibSurface.h"
#include "Renderer/Render2D.h"

#define LUA_AUTOREPLICATE

/*Base surface.* library*/
static bool Check2DRenderingContext(lua_State *L) {
	LuaInterface Lua = LuaInterface::Get(L);
	if (!URender2D::CheckContext()) {
		Lua.ThrowError("Not in a 2D rendering context.");
		return false;
	}
	return true;
}

DECLARE_LUA_FUNC(SurfaceLoadFont)
	uint32 i = 0;
	if (Lua.IsNil(3)) {
		i = URender2D::LoadFont(Lua.CheckString(1), Lua.CheckInt(2));
	} else {
		i = URender2D::LoadFont(Lua.CheckString(1), Lua.CheckInt(2), FName(*Lua.CheckString(3)));
	}	
	Lua.PushInt(i);
	return 1;
}
DECLARE_LUA_FUNC(SurfaceLoadTexture)
	uint32 i = URender2D::LoadTexture(Lua.CheckString(1));
	Lua.PushInt(i);
	return 1;
}
DECLARE_LUA_FUNC(SurfaceUnloadFont)
	URender2D::UnloadFont(Lua.CheckInt(1));
	return 0;
}
DECLARE_LUA_FUNC(SurfaceUnloadTexture)
	URender2D::UnloadTexture(Lua.CheckInt(1));
	return 0;
}
#undef LUA_AUTOREPLICATE
#define LUA_AUTOREPLICATE if (!Check2DRenderingContext(L)) { return 0; }
DECLARE_LUA_FUNC(SurfaceSetColor)
	if (Lua.IsNil(4)) {
		URender2D::SetColor(FColor(Lua.CheckInt(1), Lua.CheckInt(2), Lua.CheckInt(3)));
	} else {
		URender2D::SetColor(FColor(Lua.CheckInt(1), Lua.CheckInt(2), Lua.CheckInt(3), Lua.CheckInt(4)));
	}
	return 0;
}
DECLARE_LUA_FUNC(SurfaceSetTexture)
	URender2D::SetTexture(Lua.CheckInt(1));
	return 0;
}
DECLARE_LUA_FUNC(SurfaceResetTexture)
	URender2D::ResetTexture();
	return 0;
}
DECLARE_LUA_FUNC(SurfaceSetFont)
	URender2D::SetFont(Lua.CheckInt(1));
	return 0;
}
DECLARE_LUA_FUNC(SurfaceDrawRect)
	URender2D::DrawRect(Lua.CheckFloat(1), Lua.CheckFloat(2), Lua.CheckFloat(3), Lua.CheckFloat(4));
	return 0;
}
DECLARE_LUA_FUNC(SurfaceDrawOutlineRect)
	URender2D::DrawOutlineRect(Lua.CheckFloat(1), Lua.CheckFloat(2), Lua.CheckFloat(3), Lua.CheckFloat(4), Lua.CheckFloat(5));
	return 0;
}
DECLARE_LUA_FUNC(SurfaceSetScissorRect)
	URender2D::SetScissorRect(Lua.CheckInt(1), Lua.CheckInt(2), Lua.CheckInt(3), Lua.CheckInt(4));
	return 0;
}
DECLARE_LUA_FUNC(SurfaceDrawText)
	URender2D::DrawText(Lua.CheckString(1), Lua.CheckFloat(2), Lua.CheckFloat(3), Lua.CheckInt(4));
	return 0;
}
DECLARE_LUA_FUNC(SurfaceGetTextSize)
	float w;
	float h;
	URender2D::GetTextSize(Lua.CheckString(1), w, h);
	Lua.PushFloat(w);
	Lua.PushFloat(h);
	return 2;
}
DECLARE_LUA_FUNC(SurfaceSetFontScale)
	URender2D::SetFontScale(Lua.CheckFloat(1), Lua.CheckFloat(2));
	return 0;
}
DECLARE_LUA_FUNC(SurfaceDrawLine)
	URender2D::DrawLine(Lua.CheckFloat(1), Lua.CheckFloat(2), Lua.CheckFloat(3), Lua.CheckFloat(4), Lua.CheckFloat(5));
	return 0;
}
DECLARE_LUA_FUNC(SurfaceDrawRoundedRect)
	URender2D::DrawRoundedRect(Lua.CheckFloat(1), Lua.CheckFloat(2), Lua.CheckFloat(3), Lua.CheckFloat(4), Lua.CheckInt(5));
	return 0;
}
//FUTURE METHODS : NEEDS UE4 C++ TRICKS
DECLARE_LUA_FUNC(SurfaceDrawPoly)
	//Not sure that the function works (lua side as in all cases URender2D has empty function body for DrawPoly)...
	Lua.ArgumentCheck(Lua.GetType(1) == TABLE, 1, "expected table");
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
DECLARE_LUA_FUNC(SurfaceDrawCircle)
	URender2D::DrawCircle(Lua.CheckFloat(1), Lua.CheckFloat(2), Lua.CheckInt(3));
	return 0;
}
/*End*/

#undef LUA_AUTOREPLICATE

void LuaLibSurface::RegisterLib(LuaEngine *Lua)
{
	Lua->BeginLibReg("surface");
	Lua->AddLibFunction("LoadFont", LUA_SurfaceLoadFont);
	Lua->AddLibFunction("LoadTexture", LUA_SurfaceLoadTexture);
	Lua->AddLibFunction("UnloadFont", LUA_SurfaceUnloadFont);
	Lua->AddLibFunction("UnloadTexture", LUA_SurfaceUnloadTexture);
	Lua->AddLibFunction("SetColor", LUA_SurfaceSetColor);
	Lua->AddLibFunction("SetTexture", LUA_SurfaceSetTexture);
	Lua->AddLibFunction("ResetTexture", LUA_SurfaceResetTexture);
	Lua->AddLibFunction("SetFont", LUA_SurfaceSetFont);
	Lua->AddLibFunction("DrawRect", LUA_SurfaceDrawRect);
	Lua->AddLibFunction("DrawOutlineRect", LUA_SurfaceDrawOutlineRect);
	Lua->AddLibFunction("SetScissorRect", LUA_SurfaceSetScissorRect);
	Lua->AddLibFunction("DrawText", LUA_SurfaceDrawText);
	Lua->AddLibFunction("GetTextSize", LUA_SurfaceGetTextSize);
	Lua->AddLibFunction("SetFontScale", LUA_SurfaceSetFontScale);
	Lua->AddLibFunction("DrawLine", LUA_SurfaceDrawLine);
	Lua->AddLibFunction("DrawCircle", LUA_SurfaceDrawCircle);
	Lua->AddLibFunction("DrawRoundedRect", LUA_SurfaceDrawRoundedRect);
	Lua->AddLibFunction("DrawPoly", LUA_SurfaceDrawPoly);
	Lua->CreateLibrary();

	Lua->BeginLibReg("DrawEnums");
	Lua->AddLibConstant("TEXT_ALIGN_CENTER", 1);
	Lua->AddLibConstant("TEXT_ALIGN_LEFT", 0);
	Lua->AddLibConstant("TEXT_ALIGN_RIGHT", 2);
	Lua->CreateLibrary();
}

bool LuaLibSurface::IsClientOnly()
{
	return true;
}