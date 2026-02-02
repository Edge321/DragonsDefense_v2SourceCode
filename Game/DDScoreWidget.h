// Copyright (c) 2025, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDScoreWidget.generated.h"

/**
 * This will most likely be replaced by the soul shop widget
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDScoreWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintImplementableEvent)
	void SetScoreText(int32 EnemiesKilled);
	UFUNCTION(BlueprintImplementableEvent)
	void SetFinalScoreText(int32 EnemiesKilled);

};
