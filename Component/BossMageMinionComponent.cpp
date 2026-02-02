// Copyright (c) 2025, Edge Cope Corp. All rights reserved


#include "BossMageMinionComponent.h"
#include "Components/ArrowComponent.h"
//My classes
#include "DragonsDefense_v2/Characters/Enemy.h"
#include "DragonsDefense_v2/Characters/DDEnemyDataAsset.h"
#include "DragonsDefense_v2/Projectile/DDProjectile.h"
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"

UBossMageMinionComponent::UBossMageMinionComponent()
{
	DirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
}

void UBossMageMinionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ProjectileClass == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("%s does not have a ProjectileClass, skipping shooting functionality..."), *GetName())
		return;
	}

	//Calculating correct direction for projectile (based on where component is facing (use arrow for reference))
	FVector RotationVector = GetComponentRotation().Vector();
	FVector ProjectileVelocity = RotationVector * ProjectileSpeed;
	ActualProjectileVelocity = FVector(ProjectileVelocity.X, ProjectileVelocity.Y, 0);

	EnemyData = EnemyClass.GetDefaultObject()->EnemyData;

	if (EnemyData) {
		GetWorld()->GetTimerManager().SetTimer(ShootTimerHandler, this, &UBossMageMinionComponent::ShootProjectile, EnemyData->AttackSpeed, true);
	}
}

void UBossMageMinionComponent::OnRegister()
{
	Super::OnRegister();

	if (DirectionArrow) {
		DirectionArrow->SetupAttachment(this);
	}
}

void UBossMageMinionComponent::DisableMinion()
{
	if (bHiddenInGame) {
		UE_LOG(LogTemp, Warning, TEXT("%s has already been disabled!"), *GetName())
	}

	bHiddenInGame = true;
	GetWorld()->GetTimerManager().ClearTimer(ShootTimerHandler);
}

void UBossMageMinionComponent::ShootProjectile()
{
	ADDProjectile* Proj = GetWorld()->SpawnActor<ADDProjectile>(ProjectileClass, GetComponentLocation(), GetComponentRotation());

	if (Proj) {
		Proj->SetProjectileOwner(GetAttachmentRootActor());
		Proj->SetVelocity(ActualProjectileVelocity);
		Proj->SetCollisionChannelToIgnore(ECC_EnemyChannel);
		Proj->SetCollisionChannelToIgnore(ECC_EnemyRadiusChannel);
		Proj->SetCollisionChannelToIgnore(ECC_AttackRadiusChannel);
		Proj->SetDamage(EnemyData->Damage);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("ERROR: Enemy projectile unable to spawn"))
	}
}
