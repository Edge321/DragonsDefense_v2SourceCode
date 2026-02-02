// Copyright (c) 2025, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "BossMageMinionComponent.generated.h"

class UArrowComponent;
class UDDEnemyDataAsset;
class ADDProjectile;
class AEnemy;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UBossMageMinionComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	UBossMageMinionComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* DirectionArrow;
	
	//Which enemy is the minion component emulating?
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	TSubclassOf<AEnemy> EnemyClass;
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	TSubclassOf<ADDProjectile> ProjectileClass;
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float ProjectileSpeed = 500.0f;

	FTimerHandle ShootTimerHandler;

public:

	virtual void OnRegister() override;
	//Hides it and stops it from shooting, if it initially was
	void DisableMinion();

private:

	void ShootProjectile();

	FVector ActualProjectileVelocity;

	//Fetched from EnemyClass variable
	UDDEnemyDataAsset* EnemyData;
};
