#include "UMod.h"
#include "Entity.h"

void FPhysObj::SetGravityScale(float f)
{
	GravityScale = f;
	if (GravityScale == 0) {
		PhysComp->SetEnableGravity(false);
	}
	else if (GravityScale == 1) {
		PhysComp->SetEnableGravity(true);
	}
}

float FPhysObj::GetGravityScale()
{
	return GravityScale;
}

void FPhysObj::SetMassScale(float f)
{
	PhysComp->SetMassScale(NAME_None, f);
}

float FPhysObj::GetMassScale()
{
	return PhysComp->GetMassScale(NAME_None);
}

void FPhysObj::Freeze()
{
	PhysComp->SetSimulatePhysics(false);
}

void FPhysObj::UnFreeze()
{
	PhysComp->SetSimulatePhysics(true);
}

void FPhysObj::UpdateObj()
{
	UMOD_STAT(PHYSICSPhysObj);

	if (GravityScale < 1 && GravityScale > 0) {
		APhysicsVolume *v = PhysComp->GetPhysicsVolume();
		float grav = -v->GetGravityZ() * PhysComp->GetMass() * (1 - GravityScale);
		FVector GravityVec = FVector(0, 0, grav);
		PhysComp->AddForce(GravityVec);
	}
}