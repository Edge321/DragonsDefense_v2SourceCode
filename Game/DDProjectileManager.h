// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDProjectileManager.generated.h"

class UBillboardComponent;
class ADDProjectile;

/**
 * Does what the class's name says
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDProjectileManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADDProjectileManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* ManagerIcon;

public:
	void AddProjectileToPool(ADDProjectile* Proj);

private:
	TArray<ADDProjectile*> ProjectilePool;

	void RemoveProjectileFromPool(ADDProjectile* Proj);
	/**
	 * @brief Cleans all elements that are in the projectile
	 * pool and destroys them if they still exist
	 * 
	 */
	void CleanPool();

	UFUNCTION()
	void GameOverEventFunction();
	UFUNCTION()
	void WaveOverEventFunction();
};
