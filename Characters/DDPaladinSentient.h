// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/DDSentientPlaceable.h"
#include "DDPaladinSentient.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDPaladinSentient : public ADDSentientPlaceable
{
	GENERATED_BODY()

public:

protected:

	void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Evil")
	float TimeToBeEvil = 5.0f;

public:

	void ConvertToEvil() override;

private:

	void RevertToNormal();

	FTimerHandle EvilTimer;
	
};
