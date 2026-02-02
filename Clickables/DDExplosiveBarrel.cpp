// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDExplosiveBarrel.h"
//My classes
#include "DragonsDefense_v2/Characters/Enemy.h"
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"

void ADDExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(ExplodeTimer, this, &ADDExplosiveBarrel::Explode, TimeToExplode, false);
}

void ADDExplosiveBarrel::Explode()
{
	TArray<AActor*> IgnoreActors;
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams SphereParams(TEXT("BarrelExplosion"), false, this);

	FCollisionObjectQueryParams QueryParams;

	QueryParams.AddObjectTypesToQuery(ECC_PlaceableChannel);

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
		if (LivingActor && !LivingActor->IsA<AEnemy>()) {
			LivingActor->UpdateHealth(-ExplosionDamage);
		}
	}

	OnDeath();
}
