// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDDebuffAreaManager.generated.h"

class ADDDebuffAreaBase;
class UBillboardComponent;

UCLASS()
class DRAGONSDEFENSE_V2_API ADDDebuffAreaManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADDDebuffAreaManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Icon")
	UBillboardComponent* ManagerIcon;

	void AddToPool(ADDDebuffAreaBase* DebuffArea);
	void RemoveFromPool(ADDDebuffAreaBase* DebuffArea);
	void CleanPool();

	UFUNCTION()
	void GameOverEventFunction();

public:

	ADDDebuffAreaBase* SpawnDebuffArea(TSubclassOf<ADDDebuffAreaBase> DebuffAreaClass, FVector Location, 
		FRotator Rotation = FRotator::ZeroRotator, bool bSpawnOnFloor = false);

private:

	TArray<ADDDebuffAreaBase*> DebuffAreaPool;

};
