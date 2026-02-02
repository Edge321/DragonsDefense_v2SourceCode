// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlayerProjectile.h"
//My classes
#include "DragonsDefense_v2/Characters/LivingActor.h"

void ADDPlayerProjectile::BeginPlay()
{
	Super::BeginPlay();

	Collider->SetBoxExtent(BoxSize);
}

void ADDPlayerProjectile::HandleHit(ALivingActor* Living)
{
	if (RecentlyHitActor.IsValid() && RecentlyHitActor == Living) {
		//do nothing
	}
	else {
		float TempDamage = ProjectileDamage;
		RecentlyHitActor = MakeWeakObjectPtr(Living);
		float RandomCritChance = FMath::RandRange(0.0f, 100.0f);
		TempDamage = (bIsACriticalProjectile) ? TempDamage * CritMultiplier : TempDamage;
		Living->UpdateHealth(TempDamage);
		if (--MaxObjectsToPenetrate <= 0) {
			DestroySelf();
		}
		else {
			FVector MeshScale = Mesh->GetRelativeScale3D();
			//Scale assumed to be uniform
			Mesh->SetWorldScale3D(MeshScale * SizeDecreaseFactor);
		}
	}
}

void ADDPlayerProjectile::SetCritProjectileChance(const float CritChance)
{
	CritProjectileChance = CritChance;
}

void ADDPlayerProjectile::SetProjectileSize(const float Size)
{
	FVector MeshSize = Mesh->GetStaticMesh()->GetBoundingBox().GetSize();
	FVector DesiredMeshSize(Size, Size, Size);
	FVector ScaleNeeded = DesiredMeshSize / MeshSize;
	Mesh->SetWorldScale3D(ScaleNeeded);
	//Collider doesnt need to be adjusted since it is a child of mesh
}

void ADDPlayerProjectile::CalculateCritChance()
{
	float RandomCritChance = FMath::RandRange(0.0f, 100.0f);
	bIsACriticalProjectile = CritProjectileChance > RandomCritChance;
	if (bIsACriticalProjectile) {
		Mesh->SetMaterial(0, CriticalMaterial);
	}
}
