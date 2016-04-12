#include "UMod.h"
#include "Lua/Interface/LuaPreprocessor.h"

struct DefineToken {
	FString Name;
	FString RealName;
};

static FString LastPreProcessorError;
static TArray<DefineToken> DefinesTokens;

bool LuaPreprocessor::ProcessFile(FString fileName)
{
	FString FileContentModified;
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	IFileHandle* FileHandle = PlatformFile.OpenRead(*fileName);
	if (FileHandle) {
		FString CurLine;
		uint8 CurByte;
		while (FileHandle->Read(&CurByte, 1)) {
			if (CurByte == '\n') {
				if (!ProcessLine(CurLine)) {
					delete FileHandle;
					return false;
				}
				FileContentModified += CurLine;
				CurLine.Empty();
			} else {
				CurLine.AppendChar(CurByte);
			}
		}
	}
	return true;
}

bool LuaPreprocessor::ProcessString(FString str)
{
	TArray<FString> Lines;
	str.ParseIntoArrayLines(Lines);
	for (int i = 0; i < Lines.Num(); i++) {
		if (!ProcessLine(Lines[i])) {
			return false;
		}
	}
	return true;
}

bool LuaPreprocessor::ProcessLine(FString &line)
{
	if (line.StartsWith("#define")) { //Currently only #define
		line.RemoveFromStart("#define");
		TArray<FString> DefineData;
		line.ParseIntoArray(DefineData, TEXT(" "));
		if (DefineData.Num() < 2) {
			LastPreProcessorError = "Unexpected end of line";
			return false;
		}
		FString def;
		for (int i = 1; i < DefineData.Num(); i++) {
			def += DefineData[i];
		}
		DefineToken tok;
		tok.Name = DefineData[0];
		tok.RealName = def;
		DefinesTokens.Add(tok);
		line.Empty();
	} else {
		for (int i = 0; i < DefinesTokens.Num(); i++) {
			
		}
	}
	return true;
}

void LuaPreprocessor::ReplaceEscapeQuotes(FString &Source, FString Search)
{
	for (int i = 0; i < Source.Len(); i++) {
		Source.Mid(i, i);
	}
}

FString LuaPreprocessor::GetLastError()
{
	return LastPreProcessorError;
}