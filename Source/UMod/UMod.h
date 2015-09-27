// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#ifndef __UMOD_H__
#define __UMOD_H__

#include "Engine.h"
#include "UnrealNetwork.h"
#include "Online.h"

DECLARE_LOG_CATEGORY_EXTERN(UMod_Game, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(UMod_Maps, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(UMod_Input, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(UMod_Lua, Log, All);

template <typename ObjClass>
static FORCEINLINE ObjClass* LoadObjFromPath(const FName& Path)
{
	if (Path == NAME_None) return NULL;
	//~

	return Cast<ObjClass>(StaticLoadObject(ObjClass::StaticClass(), NULL, *Path.ToString()));
}

#endif
