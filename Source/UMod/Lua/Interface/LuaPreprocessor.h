#pragma once
#include "UMod.h"

//Yes right, I'm going to create a pre-processor for Lua
class LuaPreprocessor {
public:
	static bool ProcessFile(FString fileName);
	static bool ProcessString(FString str);
	static FString GetLastError();
private:
	static bool ProcessLine(FString &line);
	static void ReplaceEscapeQuotes(FString &Source, FString Search);
};