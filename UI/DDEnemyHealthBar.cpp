// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDEnemyHealthBar.h"
#include "Components/ProgressBar.h"

void UDDEnemyHealthBar::SetPercent(const float Percentage)
{
	if (ProgressBarRef) {
		ProgressBarRef->SetPercent(Percentage);
	}
}

void UDDEnemyHealthBar::SetHealthScale(const float Health) 
{
	float HealthBarScaleX = HealthBarScaleByHealth * Health;
	if (HealthBarScaleX > HealthBarScaleThreshold) {
		const float RemainingHealthToScale = (HealthBarScaleX - HealthBarScaleThreshold) / HealthBarScaleByHealth;
		HealthBarScaleX = HealthBarScaleThreshold + (RemainingHealthToScale * HealthBarSlowScaleByHealth);
	}

	SetRenderScale(FVector2D(HealthBarScaleX, GetRenderTransform().Scale.Y));
}
