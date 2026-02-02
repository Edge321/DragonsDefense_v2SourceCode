// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/PlaceableStates/DDSentientStateInterface.h"
#include "DDNormalSentientState.generated.h"

class AEnemy;
class ADDSentientPlaceable;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDNormalSentientState : public UDDSentientStateInterface
{
	GENERATED_BODY()
	
public:

	void OnInitialize(AActor* Owner) override;
	void HandleBeginOverlap(AActor* Actor) override;
	void HandleEndOverlap(AActor* Actor) override;
	void OnUninitialize() override;

private:

	void StoreAnyEnemy(TArray<AActor*> Actors);
	void BeginAttack();
	AEnemy* FindMinMaxEnemy(const bool IsMax, std::function<float(AEnemy*)> ValueFunc) const;

	uint8 RobinIndex;
	TArray<AEnemy*> EnemiesInArea;

};
