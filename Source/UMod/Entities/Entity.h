#pragma once
#include "UMod.h"

enum ECollisionType {
	COLLISION_NONE, //No collisions
	COLLISION_PHYSICS, //Collides with everything used for physics simulation
	COLLISION_NOT_PLAYER, //Collides with everything but not player
	COLLISION_WORLD_ONLY //Collides only with non dynamic entities
};
enum EWaterLevel {
	FULL_SUBMERGED,
	HALF_SUBMERGED,
	NULL_SUMBERGED
};

//ServerSide only structure
struct FPhysObj {
	UPrimitiveComponent *PhysComp;
	float GravityScale;

	FPhysObj(UPrimitiveComponent *Phys) {
		PhysComp = Phys;
	}

	//Call that yourself to update that physics object
	void UpdateObj();
	//Sets the velocity of that object
	void SetVelocity(FVector NewVel);
	//Sets the angle velocity of that object
	void SetAngleVelocity(FRotator NewAngleVel);
	void AddForceOffset(FVector Offset, FVector Force);
	void AddForceCenter(FVector Force);
	void SetGravityScale(float f);
	void SetMassScale(float f);
	float GetGravityScale();
	float GetMassScale();
	void Freeze();
	void UnFreeze();
	bool IsFrozen();
};

#define AUTO_NWVARS_HEADER() \
public: \
virtual void SetNWInt(FString id, int i); \
virtual void SetNWString(FString id, FString str); \
virtual void SetNWUnsined(FString id, uint32 i); \
virtual void SetNWFloat(FString id, float f); \
virtual int GetNWInt(FString id); \
virtual FString GetNWString(FString id); \
virtual uint32 GetNWUnsined(FString id); \
virtual float GetNWFloat(FString id); \
virtual void RemoveNWVar(FString id) \

#define AUTO_NWVARS_REP_CODE(UE4Class) \
DOREPLIFETIME(UE4Class, NWInts); \
DOREPLIFETIME(UE4Class, NWStrings); \
DOREPLIFETIME(UE4Class, NWUInts); \
DOREPLIFETIME(UE4Class, NWFloats) \

#define AUTO_NWVARS_BODY(UE4Class) \
void UE4Class::SetNWInt(FString id, int i) \
{ \
	NWInts.Add(id, i); \
} \
void UE4Class::SetNWString(FString id, FString str) \
{ \
	NWStrings.Add(id, str); \
} \
void UE4Class::SetNWUnsined(FString id, uint32 i) \
{ \
	NWUInts.Add(id, i); \
} \
void UE4Class::SetNWFloat(FString id, float f) \
{ \
	NWFloats.Add(id, f); \
} \
int UE4Class::GetNWInt(FString id) \
{ \
	int* i = NWInts.Find(id); \
	if (i != NULL) { \
		return *i; \
	} else { \
		return 0; \
	} \
} \
FString UE4Class::GetNWString(FString id) \
{ \
	FString* i = NWStrings.Find(id); \
	if (i != NULL) { \
		return *i; \
	} else { \
		return ""; \
	} \
} \
uint32 UE4Class::GetNWUnsined(FString id) \
{ \
	uint32* i = NWUInts.Find(id); \
	if (i != NULL) { \
		return *i; \
	} else { \
		return 0; \
	} \
} \
float UE4Class::GetNWFloat(FString id) \
{ \
	float* i = NWFloats.Find(id); \
	if (i != NULL) { \
		return *i; \
	} else { \
		return 0; \
	} \
} \
void UE4Class::RemoveNWVar(FString id) \
{ \
	if (NWInts.Contains(id)) { \
		NWInts.Remove(id); \
	} \
	if (NWFloats.Contains(id)) { \
		NWFloats.Remove(id); \
	} \
	if (NWStrings.Contains(id)) { \
		NWStrings.Remove(id); \
	} \
	if (NWUInts.Contains(id)) { \
		NWUInts.Remove(id); \
	} \
} \

/* This is the basic abstract non-instantiable class to extend for any Actor/Unreal Entity that can be cnnected to Lua */
class Entity {
public:
	//Basics
	virtual void Remove() = 0;
	virtual FString GetClass() = 0;
	virtual int EntIndex() = 0;
	//End

	//@LuaSystem
	virtual int GetLuaRef() = 0;
	virtual void SetLuaRef(int r) = 0;
	virtual void LuaUnRef() = 0;
	virtual void SetLuaClass(FString s) = 0;
	//End

	//Position/Angles
	virtual void SetPos(FVector vec) = 0;
	virtual void SetAngles(FRotator ang) = 0;
	virtual FVector GetPos() = 0;
	virtual FRotator GetAngles() = 0;
	//End

	//Model/Color
	virtual void SetModel(FString mdl) = 0;
	virtual FString GetModel() = 0;
	virtual void SetColor(FColor col) = 0;
	virtual FColor GetColor() = 0;
	//End

	//Physics/Collisions
	virtual void AddPhysicsObject() = 0; //ServerSide
	virtual FPhysObj *GetPhysicsObject() = 0; //ServerSide
	virtual void SetCollisionModel(ECollisionType collision) = 0; //ServerSide
	virtual ECollisionType GetCollisionModel() = 0;
	virtual EWaterLevel GetWaterLevel() = 0;
	//End

	//Materials
	virtual void SetMaterial(FString path) = 0; //No sync possible : UE4 does not allow c array replication
	virtual void SetSubMaterial(int32 index, FString path) = 0; //No sync possible : UE4 does not allow c array replication
	virtual FString GetMaterial() = 0;
	virtual FString GetSubMaterial(int32 index) = 0;
	virtual int32 GetSubMaterialsNum() = 0;
	//End

	//Network (For Lua)
	virtual void SetNWInt(FString id, int i) = 0;
	virtual void SetNWString(FString id, FString str) = 0;
	virtual void SetNWUnsined(FString id, uint32 i) = 0;
	virtual void SetNWFloat(FString id, float f) = 0;
	virtual int GetNWInt(FString id) = 0;
	virtual FString GetNWString(FString id) = 0;
	virtual uint32 GetNWUnsined(FString id) = 0;
	virtual float GetNWFloat(FString id) = 0;
	virtual void RemoveNWVar(FString id) = 0;
	//End	
};