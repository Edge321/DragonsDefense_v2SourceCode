// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/DDTrapPlaceable.h"
#include "DDStoneFatigueTrap.generated.h"

class ADDDebuffAreaBase;
class ADDGameModeBase;
class AEnemy;
class USphereComponent;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDStoneFatigueTrap : public ADDTrapPlaceable
{
	GENERATED_BODY()
	
public:

	ADDStoneFatigueTrap();

protected:

	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	void HandleBeginOverlap(AActor* Actor) override;
	void HandleEndOverlap(AActor* Actor) override;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* DebuffRadiusMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* DebuffRadiusCollider;

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Debuff")
	TSubclassOf<ADDDebuffAreaBase> DebuffAreaClass;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Debuff")
	float DebuffDetectionRadius = 300.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Debuff")
	float SlownessMultiplier = 0.8f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Debuff")
	float TimeToSpawnSlowness = 3.0f;
	//Where to spawn debuff area based on offset
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Debuff")
	FVector DebuffAreaOffset = FVector::Zero();

public:

	void BeginDebuffSpawn();
	void StopDebuffSpawn();

private:

	void SpawnDebuffArea();
	const FVector GetRadiusMeshSize() const;

	bool bIsSpawningDebuffs = false;
	FTimerHandle SpawnDebuffTimer;
	TWeakObjectPtr<ADDGameModeBase> GameModeRef;
	TArray<TWeakObjectPtr<AEnemy>> EnemiesInArea;

};
