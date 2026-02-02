// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDDisrupterEnemy.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/Managers/DDCraterManager.h"
#include "DragonsDefense_v2/Game/DDEnemySpawner.h"
#include "DragonsDefense_v2/Characters/DDPlayer.h"
#include "DragonsDefense_v2/Helpers/DDCrater.h"

void ADDDisrupterEnemy::BeginPlay()
{
	Super::BeginPlay();

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->OnWaveStart.AddDynamic(this, &ADDDisrupterEnemy::WaveStartEventFunction);
		ADDPlayer& ThePlayer = GameMode->GetPlayer();
		if (IsValid(&ThePlayer)) {
			PlayerLocation = ThePlayer.GetActorLocation();
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("%s unable to fetch Player location. Enemy might be for initialization."), *GetName())
		}
		ADDEnemySpawner& EnemySpawner = GameMode->GetEnemySpawner();
		if (IsValid(&EnemySpawner)) {
			SpawnerLocation = EnemySpawner.GetActorLocation();
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("%s unable to fetch EnemySpawner location. Enemy might be for initialziation."), *GetName())
		}
	}

	RandomizeDisruptionLocation();
}

void ADDDisrupterEnemy::Tick(float DeltaTime)
{
	if (bStartMoving) {
		if (bLeavingBattlefield) {
			StartMoving();
			FVector ActualMovement = FVector(DeltaTime * MovementSpeed, 0, 0);
			FloatingPawnMovement->AddInputVector(ActualMovement);
		}
		else {
			FVector ActualMovement = FVector(DeltaTime * MovementSpeed * -1.0, 0, 0);
			FloatingPawnMovement->AddInputVector(ActualMovement);

			if (CheckDistance()) {
				StopMoving();
				PlaceDisruption();
			}
		}
	}
}

void ADDDisrupterEnemy::RandomizeDisruptionLocation()
{
	float LimitX = DisruptionSize.X / 2;
	float LimitY = DisruptionSize.Y / 2;

	float LowAreaLimitX = PlayerLocation.X;
	float HighAreaLimitX = SpawnerLocation.X;

	float CorrectedLowAreaLimitX = FMath::Clamp(LowAreaLimitX + LimitX, LowAreaLimitX, HighAreaLimitX);
	float CorrectedHighAreaLimitX = FMath::Clamp(HighAreaLimitX - LimitX, LowAreaLimitX, HighAreaLimitX);
	float CorrectedAreaLimitY = FMath::Clamp(AreaLimitY - LimitY, 0, AreaLimitY);

	DisruptionLocation.X = FMath::RandRange(CorrectedLowAreaLimitX, CorrectedHighAreaLimitX);
	DisruptionLocation.Y = FMath::RandRange(-CorrectedAreaLimitY, CorrectedAreaLimitY);
}

const FVector ADDDisrupterEnemy::GetDisruptionLocation() const
{
	return DisruptionLocation;
}

const FVector ADDDisrupterEnemy::GetDisruptionSize() const
{
	return DisruptionSize;
}

void ADDDisrupterEnemy::PlaceDisruption()
{
	//Execute only once
	if (!bIsPlacingDisruption) {
		bIsPlacingDisruption = true;
		ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GameMode) {
			ADDCrater* Disruption = GameMode->GetCraterManager().SpawnCrater(DisruptionClass, DisruptionLocation);
			FVector MeshSize = Disruption->GetMeshSize();
			FVector DesiredScale = DisruptionSize / MeshSize;
			Disruption->SetActorScale3D(DesiredScale);
		}
		bLeavingBattlefield = true;
		GetWorldTimerManager().SetTimer(LeavingLifeTime, this, &ADDDisrupterEnemy::OnDisappear, LeavingTime);
	}
}

void ADDDisrupterEnemy::OnDisappear()
{
	OnEnemyDeath.ExecuteIfBound(this);
}

void ADDDisrupterEnemy::WaveStartEventFunction()
{
	bStartMoving = true;
}
