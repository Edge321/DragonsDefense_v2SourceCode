// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDJumboEnemy.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/DDEnemySpawner.h"

void ADDJumboEnemy::OnDeath()
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->AddScore(Score);
		GameMode->UpdateSouls(SoulValue);
	}
	
	Explode();
	OnEnemyDeath.ExecuteIfBound(this);
}

void ADDJumboEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Explode near castle
	if (CheckDistance()) {
		Explode();
		OnEnemyDeath.ExecuteIfBound(this);
	}
}

void ADDJumboEnemy::Explode() const
{
	ADDEnemySpawner* Spawner;
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		Spawner = &(GameMode->GetEnemySpawner());
		int32 RandomSpawn = FMath::RandRange(LowEnemySpawn, HighEnemySpawn);
		Spawner->SpawnEnemies(EnemyExplodies, RandomSpawn, GetActorLocation());
	}
	else {
		UE_LOG(LogTemp, Fatal, TEXT("How did we get here?"))
		return;
	}
}
