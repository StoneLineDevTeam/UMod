#pragma once
#include "UMod.h"

class UMOD_API IUModPlatformUtils {
public:
	virtual void* GetConsoleHandle() = 0;
	virtual void SetConsoleTitleString(void* ConsoleHandle, FString text) = 0;
	virtual void SetConsoleIcon(void* ConsoleHandle, void* Icon) = 0;
	virtual void* LoadIconFromFile(FString path, int width, int height) = 0;
	virtual void SetWindowIcon(void* Handle, void* Icon) = 0;
	virtual void BindStdIO(void* ConsoleHandle) = 0;
	virtual void DisableWindowResize(void* Handle) = 0;
};

class FSTDInputThread : public FRunnable {
	static  FSTDInputThread* Runnable;
	FRunnableThread* Thread;

public:
	FSTDInputThread();
	~FSTDInputThread();

	// Begin FRunnable interface.
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	// End FRunnable interface

	static void StartThread();
	static void EndThread();
};

#if PLATFORM_WINDOWS
#include "UModWindows.h"
#define FUModPlatformUtils WindowsHelper
#elif PLATFORM_MAC
//TODO : Add Mac implementation
#elif PLATFORM_LINUX
//TODO : Add Linux implementation
#endif