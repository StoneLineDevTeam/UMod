//#include "UMod.h"
//#include "UModWindows.h"

#include "AllowWindowsPlatformTypes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <thread>
#include <iostream>

inline void* WindowsHelper::GetConsoleHandle()
{
	HWND UE4Console = GetConsoleWindow();
	return (void*)UE4Console;
}

inline void WindowsHelper::SetConsoleTitleString(void* ConsoleHandle, FString text)
{
	HWND UE4Console = (HWND)ConsoleHandle;
	SetWindowText(UE4Console, *text);
}

inline void WindowsHelper::SetConsoleIcon(void* ConsoleHandle, void* Icon)
{
	HWND UE4Console = (HWND)ConsoleHandle;
	HICON icon = (HICON)Icon;
	SendMessage(UE4Console, WM_SETICON, ICON_SMALL, (LPARAM)icon);
	SendMessage(UE4Console, WM_SETICON, ICON_BIG, (LPARAM)icon);
}

inline void* WindowsHelper::LoadIconFromFile(FString path, int width, int height)
{
	HICON icon = (HICON)LoadImage(NULL, *path, IMAGE_ICON, width, height, LR_LOADFROMFILE);
	return (void*)icon;
}

inline void WindowsHelper::SetWindowIcon(void* Handle, void* Icon)
{
	HWND win = (HWND)Handle;
	HICON icon = (HICON)Icon;
	SendMessage(win, WM_SETICON, ICON_SMALL, (LPARAM)icon);
	SendMessage(win, WM_SETICON, ICON_BIG, (LPARAM)icon);
}

inline void WindowsHelper::BindStdIO(void* ConsoleHandle)
{
	AttachConsole(GetProcessId((HWND)ConsoleHandle));
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	std::wcout.clear();
	std::cout.clear();
	std::wcerr.clear();
	std::cerr.clear();
	std::wcin.clear();
	std::cin.clear();
	std::ios::sync_with_stdio();
}

inline void WindowsHelper::DisableWindowResize(void* Handle)
{
	LONG style = GetWindowLong((HWND)Handle, GWL_STYLE);
	style = style &~ WS_SIZEBOX;
	style = style &~ WS_MAXIMIZEBOX;
	SetWindowLong((HWND)Handle, GWL_STYLE, style);
}