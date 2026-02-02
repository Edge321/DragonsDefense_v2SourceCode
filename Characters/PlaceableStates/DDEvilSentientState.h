// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/PlaceableStates/DDSentientStateInterface.h"
#include "DDEvilSentientState.generated.h"

class ADDSentientPlaceable;
class ADDPlaceable;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDEvilSentientState : public UDDSentientStateInterface
{
	GENERATED_BODY()
	
public:

	void OnInitialize(AActor* Owner) override;
	void HandleBeginOverlap(AActor* Actor) override;
	void HandleEndOverlap(AActor* Actor) override;
	void OnUninitialize() override;

private:

	void StoreAnyPlaceables(TArray<AActor*> Actors);
	void BeginAttack();

	TArray<ADDPlaceable*> PlaceablesInArea;

	float EvilAttackRadiusMultiplier = 0.5f;
	float SellingPriceBeforeEvil = 0;
	float AttackRadiusBeforeEvil = 0;

};
