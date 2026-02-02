// Copyright (c) 2025, Edge Cope Corp. All rights reserved


#include "DDNewVillagerAlertWidget.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/DDEnemySpawner.h"
#include "DragonsDefense_v2/Characters/Enemy.h"

void UDDNewVillagerAlertWidget::ActivateNotification(const int32 EnemyID)
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		PopulateAlertInfo(GameMode->GetEnemySpawner().GetEnemyEncyclopedia()[(EDDEnemyIDs)EnemyID]);
	}

	GetWorld()->GetTimerManager().SetTimer(NotificationHandle, this, &UDDNewVillagerAlertWidget::DeactiveNotification, NotificationTimer);
}

void UDDNewVillagerAlertWidget::DeactiveNotification()
{
	SetVisibility(ESlateVisibility::Hidden);
}
