// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDShrewdEnemy.h"

void ADDShrewdEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float TimeSinceStart = GetWorld()->GetTimeSeconds();

	//float MovementX = DeltaTime * MovementSpeed * -1.0;
	//float MovementY = DeltaTime * (Amplitude * (FMath::Sin(Frequency * TimeSinceStart)));

	const float MovementX = MovementSpeed * -1.0;
	const float MovementY = (Amplitude * (FMath::Sin(Frequency * TimeSinceStart)));

	const FVector ActualMovement(MovementX, MovementY, 0);

	FloatingPawnMovement->AddInputVector(ActualMovement);

	CheckDistance();
}

const FVector ADDShrewdEnemy::GetRandomInaccuracy() const
{
	FVector RandomInaccuracy;
	RandomInaccuracy.X = FMath::RandRange(-MaxInaccuracy, MaxInaccuracy);
	RandomInaccuracy.Y = FMath::RandRange(-MaxInaccuracy, MaxInaccuracy);
	RandomInaccuracy.Z = 0.0f;
	return RandomInaccuracy;
}
