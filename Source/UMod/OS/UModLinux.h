#pragma once
#include "UMod.h"

class UMOD_API LinuxHelper : public IUModPlatformUtils {
public:
	virtual void* GetConsoleHandle();
	virtual void SetConsoleTitleString(void* ConsoleHandle, FString text);
	virtual void SetConsoleIcon(void* ConsoleHandle, void* Icon);
	virtual void* LoadIconFromFile(FString path, int width, int height);
	virtual void SetWindowIcon(void* Handle, void* Icon);
	virtual void BindStdIO(void* ConsoleHandle);
	virtual void DisableWindowResize(void* Handle);
};

#include "UModLinuxImpl.h"