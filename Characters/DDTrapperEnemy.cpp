// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDTrapperEnemy.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/DDClickableManager.h"
#include "DragonsDefense_v2/Clickables/DDClickableBase.h"

void ADDTrapperEnemy::BeginPlay()
{
	Super::BeginPlay();

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameModeRef = MakeWeakObjectPtr(GameMode);
	}

	StartSpawning();
}

void ADDTrapperEnemy::StartSpawning()
{
	float RandomSpawn = FMath::RandRange(LowSpawnExplosiveInterval, HighSpawnExplosiveInterval);
	GetWorldTimerManager().SetTimer(TimeSpawnExplosive, this, &ADDTrapperEnemy::SpawnExplosiveBarrel, RandomSpawn, true);
}

void ADDTrapperEnemy::SpawnExplosiveBarrel()
{
	if (ClickableClass) {
		GameModeRef->GetClickableManager().SpawnClickable(ClickableClass, GetActorLocation());
		StartSpawning();
	}
	else {
		UE_LOG(LogTemp, Fatal, TEXT("Failed to set clickable class on %s"), *GetName())
	}
}
