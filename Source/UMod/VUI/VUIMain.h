#pragma once
#include "UMod.h"

class FVUIApp {
public:
	static FVUIApp *Get();
	//TODO : Create VFrame/VPanel classes
	void AddFrame(/*VFrame Frame*/);
	void AddRootPanel(/*VPanel Panel*/);
};

class FVUIUtils {
public:
	static bool IsMouseInRect(float x, float y, float w, float h);
	static void GetMousePos(float &x, float &y);
};