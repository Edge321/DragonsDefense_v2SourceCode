// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDTheKingEnemiesToSpawnDowngrade.h"
//My classes
#include "DragonsDefense_v2/Game/DDEnemySpawner.h"
#include "DragonsDefense_v2/Game/DDGameModeBase.h"

TWeakObjectPtr<ADDTheKing> TheKingRef;
//Comes from DowngradeAmount
EDDEnemyIDs EnemyIDToSpawn = EDDEnemyIDs::NULL_ENEMY;

void UDDTheKingEnemiesToSpawnDowngrade::ApplyDowngrade(AActor* Target, const float DowngradeAmount) const
{
	if (Target->IsA<ADDTheKing>()) {
		ADDTheKing* TheKing = Cast<ADDTheKing>(Target);
		TheKingRef = MakeWeakObjectPtr(TheKing);
		EnemyIDToSpawn = (EDDEnemyIDs) DowngradeAmount;
		std::function<void()> Func = std::bind(&UDDTheKingEnemiesToSpawnDowngrade::DowngradeFunction, this);
		TheKingRef->AddDowngradeFunction(Func);
	}
	else {
		PrintErrorLog();
	}
}

void UDDTheKingEnemiesToSpawnDowngrade::DowngradeFunction() const
{
	FDDWaveInfo CalculatedWaveInfo = TheKingRef->GetCalculatedWaveInfo();

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(TheKingRef->GetWorld()->GetAuthGameMode());
	if (GameMode) {
		ADDEnemySpawner& Spawner = GameMode->GetEnemySpawner();
		TMap<EDDEnemyIDs, TSubclassOf<AEnemy>> Encyclopedia = Spawner.GetEnemyEncyclopedia();
		if (Encyclopedia.Contains(EnemyIDToSpawn)) {
			CalculatedWaveInfo.EnemiesToSpawn.Add(Encyclopedia[EnemyIDToSpawn]);
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Error: EnemyID: %d not found, ignoring Downgrade function..."), EnemyIDToSpawn)
		}
	}

	TheKingRef->SetCalculatedWaveInfo(CalculatedWaveInfo);
}
