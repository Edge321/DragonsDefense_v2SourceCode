// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDExplosiveProjectile.h"
//My classes
#include "DragonsDefense_v2/Characters/Enemy.h"
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"

void ADDExplosiveProjectile::HandleHit(ALivingActor* Living)
{
	Explode();
}

void ADDExplosiveProjectile::Explode()
{
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams SphereParams(TEXT("ExplosiveProjectile"), false, this);

	SphereParams.AddIgnoredActor(ActorOwner);
	FCollisionObjectQueryParams QueryParams;

	QueryParams.AddObjectTypesToQuery(ECC_PlaceableChannel);
	QueryParams.AddObjectTypesToQuery(ECC_EnemyChannel);

	//Remove channels the projectile is set to ignoring
	for (ECollisionChannel Channel : ChannelsIgnored) {
		QueryParams.RemoveObjectTypesToQuery(Channel);
	}

	UWorld* World = GetWorld();
	if (World)
	{
		World->OverlapMultiByObjectType(
			Overlaps,
			GetActorLocation(),
			FQuat::Identity,
			QueryParams,
			FCollisionShape::MakeSphere(ExplosionRadius),
			SphereParams);

		DrawDebugSphere(World, GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 5.0f, 0, 2);
	}

	for (FOverlapResult Result : Overlaps) {
		ALivingActor* LivingActor = Cast<ALivingActor>(Result.GetActor());
		if (LivingActor) {
			LivingActor->UpdateHealth(ProjectileDamage);
		}
	}

	DestroySelf();
}
