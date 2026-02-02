// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/Enemy.h"
#include "DDMageSniperEnemy.generated.h"

class ADDSentientPlaceable;
class ADDGameModeBase;
class ADDPlaceableManager;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTurnDetectable, ADDMageSniperEnemy*);

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDMageSniperEnemy : public AEnemy
{
	GENERATED_BODY()

protected:

	void virtual BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float SnipeDelay = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Stats")
	float DetectionDelay = 10.0f;

public:

	void virtual Tick(float DeltaTime) override;

	FOnTurnDetectable OnTurnDetectable;

private:

	void SnipeSentient();
	void StartSniping();
	void StopSniping();
	void EnableDetection();
	TWeakObjectPtr<ADDSentientPlaceable> FindFurthestSentient(const TArray<TWeakObjectPtr<ADDSentientPlaceable>>& Sentients) const;

	bool bIsSniping = false;

	TWeakObjectPtr<ADDSentientPlaceable> SentientTarget;
	TWeakObjectPtr<ADDGameModeBase> GameModeRef;
	FTimerHandle TimeToSnipe;
	FTimerHandle TimeToBeDetected;

};
