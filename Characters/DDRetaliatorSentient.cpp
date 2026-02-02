// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDRetaliatorSentient.h"
//My classes
#include "DragonsDefense_v2/Characters/Enemy.h"
//#include "DragonsDefense_v2/Projectile/DDProjectile.h"
#include "DragonsDefense_v2/Projectile/DDSmartDebuffAreaProjectile.h"
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"

void ADDRetaliatorSentient::HandleBeingHit(ADDProjectile* ProjectileHitBy)
{
	TSubclassOf<ADDProjectile> ProjectileClass;
	ProjectileClass = ProjectileHitBy->GetClass();

	ADDProjectile* RetaliateProj = GetWorld()->SpawnActor<ADDProjectile>(ProjectileClass, GetActorLocation(), FRotator::ZeroRotator);
	if (RetaliateProj) {
		SetReturnProjectileStats(RetaliateProj, ProjectileHitBy);
	}
}

void ADDRetaliatorSentient::SetReturnProjectileStats(ADDProjectile* RetaliateProj, ADDProjectile* ProjectileHitBy)
{
	float ProjOwnerProjSpeed = 100.0f;

	const AActor* ProjOwner = ProjectileHitBy->GetProjectileOwner();
	const AEnemy* EnemyOwner = Cast<const AEnemy>(ProjOwner);

	if (EnemyOwner) {
		ProjOwnerProjSpeed = EnemyOwner->GetProjectileSpeed();
	}

	//Calculating direction of projectile, factoring in its original speed
	FVector ProjOwnerLocation = ProjOwner->GetActorLocation();
	FVector HitActorLocation = GetActorLocation();
	FVector ProjDirection = (ProjOwnerLocation - HitActorLocation);
	ProjDirection.Normalize();
	FVector CorrectProjVector = ProjDirection * ProjOwnerProjSpeed;

	RetaliateProj->SetProjectileOwner(this);
	if (EnemyOwner) {
		RetaliateProj->SetVelocity(CorrectProjVector);
	}
	else { 
		//If the retaliator was hit by an evil placeable, keep it simple for them
		//Inverse that velocity!
		RetaliateProj->SetVelocity(ProjectileHitBy->GetProjectileVelocity() * -1);
	}

	if (RetaliateProj->IsA<ADDSmartDebuffAreaProjectile>()) {
		RetaliateProj->SetDamage(RetaliateBossDamage);
	}
	else {
		RetaliateProj->SetDamage(ProjectileHitBy->GetDamage() * RetaliateDamageMultiplier);
	}
	RetaliateProj->SetCollisionChannelToIgnore(ECC_PlaceableChannel);
	RetaliateProj->SetCollisionChannelToIgnore(ECC_AttackRadiusChannel);
	RetaliateProj->SetCollisionChannelToIgnore(ECC_EnemyRadiusChannel);
}
