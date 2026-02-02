// Copyright (c) 2025, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDNewVillagerAlertWidget.generated.h"

class AEnemy;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDNewVillagerAlertWidget : public UUserWidget
{
	GENERATED_BODY()


protected:

	UFUNCTION(BlueprintCallable)
	void DeactiveNotification();
	UFUNCTION(BlueprintImplementableEvent)
	void PopulateAlertInfo(TSubclassOf<AEnemy> EnemyClass);

	UPROPERTY(EditDefaultsOnly, Category = "Timer")
	float NotificationTimer = 4.0f;
	
public:

	void ActivateNotification(const int32 EnemyID);

private:

	FTimerHandle NotificationHandle;

};
