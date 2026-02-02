// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPenetrativeProjectile.h"
//My classes
#include "DragonsDefense_v2/Characters/LivingActor.h"

void ADDPenetrativeProjectile::HandleHit(ALivingActor* Living)
{
	if (RecentlyHitActor.IsValid() && RecentlyHitActor == Living) {
		//do nothing
	}
	else {
		RecentlyHitActor = MakeWeakObjectPtr(Living);
		Living->UpdateHealth(ProjectileDamage);
		ProjectileDamage *= DamageAfterPenetrationModifier;
		if (--MaxObjectsToPenetrate <= 0) {
			DestroySelf();
		}
	}

}

const int32 ADDPenetrativeProjectile::GetMaxObjectsToPenetrate() const
{
	return MaxObjectsToPenetrate;
}

void ADDPenetrativeProjectile::SetMaxObjectsToPenetrate(const int32 MaxObjects)
{
	MaxObjectsToPenetrate = MaxObjects;
}
