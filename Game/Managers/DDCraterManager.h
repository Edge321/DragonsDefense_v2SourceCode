// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDCraterManager.generated.h"

class UBillboardComponent;
class ADDCrater;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDCraterManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADDCraterManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Icon")
	UBillboardComponent* ManagerIcon;

	void AddToPool(ADDCrater* Crater);
	void RemoveFromPool(ADDCrater* Crater);
	void CleanPool();

	UFUNCTION()
	void GameOverEventFunction();

public:

	ADDCrater* SpawnCrater(TSubclassOf<ADDCrater> CraterClass, FVector Location, FRotator Rotation = FRotator::ZeroRotator);

private:

	TArray<ADDCrater*> CraterPool;
};
