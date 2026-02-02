// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDHorizontalEnemy.h"

ADDHorizontalEnemy::ADDHorizontalEnemy()
{
	AttackSight->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADDHorizontalEnemy::OnNoBounceBack()
{
	OnEnemyDeath.ExecuteIfBound(this);
}

void ADDHorizontalEnemy::Tick(float DeltaTime)
{
	if (bIsBounced) {
		ResetPawnVelocity(DeltaTime);
	}

	FVector ActualMovement = FVector(0, DeltaTime * MovementSpeed, 0);
	FloatingPawnMovement->AddInputVector(ActualMovement);

	AdjustHealthBarRotation();
}

void ADDHorizontalEnemy::SetBounceLocation(const float LocationX)
{
	CurrentValue = FloatingPawnMovement->Velocity;
	bIsBounced = true;
	ElapsedTime = 0;
	FVector NewLocation(LocationX, GetActorLocation().Y, GetActorLocation().Z);
	SetActorLocation(NewLocation, false, nullptr, ETeleportType::TeleportPhysics);
	//No more bouncing around
	if (--BounceAmount < 0) {
		OnNoBounceBack();
	}
}

const bool ADDHorizontalEnemy::GetOverrideBounceSpawn() const
{
	return bOverrideBounceSpawn;
}

void ADDHorizontalEnemy::GetBounceIntervalSpawn(float& LowBounce, float& HighBounce) const
{
	LowBounce = LowerBounceSpawn;
	HighBounce = HighBounceSpawn;
}

void ADDHorizontalEnemy::ResetPawnVelocity(float DeltaTime)
{
	if (ElapsedTime < InterpolationDuration) {
		ElapsedTime += DeltaTime;

		float Alpha = ElapsedTime / InterpolationDuration;

		FloatingPawnMovement->Velocity = FMath::Lerp(CurrentValue, TargetValue, Alpha);
	}
	else {
		//Done with interpolation
		bIsBounced = false;
		//Time to move other way
		MovementSpeed *= -1;
	}
}
