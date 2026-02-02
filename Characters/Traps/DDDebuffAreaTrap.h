// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/DDTrapPlaceable.h"
#include "DragonsDefense_v2/DebuffArea/DDDebuffAreaBase.h"
#include "DDDebuffAreaTrap.generated.h"

class AEnemy;

/**
 * 
 */
UCLASS(Abstract)
class DRAGONSDEFENSE_V2_API ADDDebuffAreaTrap : public ADDTrapPlaceable
{
	GENERATED_BODY()

protected:

	void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Stats|DebuffArea")
	TSubclassOf<ADDDebuffAreaBase> DebuffAreaClass;

	virtual void EnableDebuffArea() PURE_VIRTUAL(ADDDebuffAreaTrap::EnableDebuffArea, );
	virtual void DisableDebuffArea() PURE_VIRTUAL(ADDDebuffAreaTrap::DisableDebuffArea, );

	void AdjustDebuffArea();
	void OnDeath() override;
	void HandleBeginOverlap(AActor* Actor) override;
	void HandleEndOverlap(AActor* Actor) override;
	
	TWeakObjectPtr<ADDDebuffAreaBase> DebuffAreaRef;
	TArray<TWeakObjectPtr<AEnemy>> EnemiesInArea;

public:

	void ConvertToEvil() override;
	
};
