// Copyright (c) 2025, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDEnemyHealthBar.generated.h"

class UProgressBar;

/**
 * This health bar originally applied to enemies, but is now used for placeables too
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDEnemyHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(BlueprintReadWrite, Category = "HealthBar")
	UProgressBar* ProgressBarRef;

	//How much does the health bar widget scale based on the amount of health of the placeable
	//Formula: HealthBarScaleY = HealthBarScaleByHealth * Health
	UPROPERTY(VisibleDefaultsOnly, Category = "HealthBar")
	float HealthBarScaleByHealth = 0.15f;
	//What is the health bar scale on the y-axis threshold until it transitions into the slow scale variable "HealthBarSlowScaleByHealth"?
	UPROPERTY(VisibleDefaultsOnly, Category = "HealthBar")
	float HealthBarScaleThreshold = 1.0f;
	UPROPERTY(VisibleDefaultsOnly, Category = "HealthBar")
	float HealthBarSlowScaleByHealth = 0.05f;

public:

	void SetPercent(const float Percentage);
	//Sets the scale of the health bar based on the health passed in
	void SetHealthScale(const float Health);

};
