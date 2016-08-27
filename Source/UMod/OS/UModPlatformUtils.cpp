#include "UMod.h"
#include "UModPlatformUtils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <io.h>
#include <iostream>

/* STDInputThread implementation */
FSTDInputThread* FSTDInputThread::Runnable = NULL;

void FSTDInputThread::StartThread()
{
	if (!Runnable && FPlatformProcess::SupportsMultithreading()) {
		Runnable = new FSTDInputThread();
	}
}

FSTDInputThread::FSTDInputThread()
{
	Thread = FRunnableThread::Create(this, TEXT("STDInputThread"), 0, TPri_BelowNormal);
}

FSTDInputThread::~FSTDInputThread()
{
	delete Thread;
	Thread = NULL;
}

void FSTDInputThread::EndThread()
{
	if (Runnable) {
		delete Runnable;
		Runnable = NULL;
	}
}

bool FSTDInputThread::Init()
{
	UE_LOG(UMod_Input, Log, TEXT("----------> STD Input Thread Started <----------"));
	return true;
}

uint32 FSTDInputThread::Run()
{
	std::string str;
	while (true) {
		std::getline(std::cin, str);
		if (!str.empty()) {
			FString cmdline = FString(ANSI_TO_TCHAR(str.c_str()));
			UE_LOG(UMod_Input, Log, TEXT("Input command line : '%s'"), *cmdline);
			UUModGameInstance::RunCMD = cmdline;
			UUModGameInstance::ShouldRunCMD = true;
		}
	}
	return 0;
}

void FSTDInputThread::Stop()
{
	UE_LOG(UMod_Input, Log, TEXT("----------> STD Input Thread Ended <----------"));
}
/* End */