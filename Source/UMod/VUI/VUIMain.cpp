#include "UMod.h"
#include "VUIMain.h"
#include "UModGameInstance.h"

//Begin FVUIUtils implementation
float FVUIUtils::MouseX = 0.0F;
float FVUIUtils::MouseY = 0.0F;

bool FVUIUtils::IsMouseInRect(float x, float y, float w, float h)
{
	return MouseX >= x && MouseX <= (x + w) && MouseY >= y && MouseY <= (y + h);
}

//Begin FVUIApp implementation
FVUIApp *FVUIApp::AppInstance = NULL;
UUModGameInstance *FVUIApp::Game = NULL;

void FVUIApp::EventKeyTyped(FKey key)
{

}

void FVUIApp::EventCharTyped(TCHAR chr)
{

}

void FVUIApp::EventMouseClick(EMouseButton but)
{

}

void FVUIApp::EventMouseWheelUp()
{

}

void FVUIApp::EventMouseWheelDown()
{

}

FVUIApp *FVUIApp::Get()
{
	return AppInstance;
}

void FVUIApp::InitVUISystem(UUModGameInstance *G)
{
	Game = G;
	AppInstance = new FVUIApp();
}