#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <fcntl.h>
#include <iostream>

inline void* LinuxHelper::GetConsoleHandle()
{
	return NULL;
}

inline void LinuxHelper::SetConsoleTitleString(void* ConsoleHandle, FString text)
{
	
}

inline void LinuxHelper::SetConsoleIcon(void* ConsoleHandle, void* Icon)
{
	
}

inline void* LinuxHelper::LoadIconFromFile(FString path, int width, int height)
{
	return NULL;
}

inline void LinuxHelper::SetWindowIcon(void* Handle, void* Icon)
{
	
}

inline void LinuxHelper::BindStdIO(void* ConsoleHandle)
{
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

inline void LinuxHelper::DisableWindowResize(void* Handle)
{
	
}