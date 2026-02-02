// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/PlaceableStates/DDSentientStateInterface.h"
#include "DDPaladinNormalState.generated.h"

class AEnemy;
class ADDPlaceable;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDPaladinNormalState : public UDDSentientStateInterface
{
	GENERATED_BODY()
	
public:

	void OnInitialize(AActor* Owner) override;
	void HandleBeginOverlap(AActor* Actor) override;
	void HandleEndOverlap(AActor* Actor) override;
	void OnUninitialize() override;
	void BeginDestroy() override;

private:

	void StoreAnyEnemyOrEvil(TArray<AActor*> Actors);
	void BeginAttack();
	void SearchForEvil();
	void CheckForEvils();
	void SearchForEnemy();
	AEnemy* FindMinMaxEnemy(const bool IsMax, std::function<float(TWeakObjectPtr<AEnemy>)> ValueFunc);

	TWeakObjectPtr<ADDPlaceable> CurrentlyTargetedEvil;
	TArray<TWeakObjectPtr<AEnemy>> EnemiesInArea;
	TArray<TWeakObjectPtr<ADDPlaceable>> EvilsInArea;
	uint8 RobinIndex;

	float SmiteEvilDamage = -9999999;
	float CheckEvilInterval = 0.5f;

	FTimerHandle CheckEvilsTimer;

};
