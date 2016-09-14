#pragma once
#include "UMod.h"

class UUModGameInstance;

enum EMouseButton {
	LEFT_CLICK,
	RIGHT_CLICK,
	MIDDLE_CLICK
};

class FVUIApp {
public:
	static FVUIApp *Get();
	static void InitVUISystem(UUModGameInstance *G);

	//TODO : Create VFrame/VPanel classes
	void AddFrame(/*VFrame Frame*/);
	void AddRootPanel(/*VPanel Panel*/);

	void EventKeyTyped(FKey key);
	void EventCharTyped(TCHAR chr);
	void EventMouseClick(EMouseButton but);
	void EventMouseWheelUp();
	void EventMouseWheelDown();
private:
	static FVUIApp *AppInstance;
	static UUModGameInstance *Game;
};

class FVUIUtils {
public:
	static bool IsMouseInRect(float x, float y, float w, float h);
	static float MouseX;
	static float MouseY;
};