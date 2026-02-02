// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/Enemy.h"
#include "DDTinkererEnemy.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDTinkererEnemy : public AEnemy
{
	GENERATED_BODY()
	
public:
	ADDTinkererEnemy();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* ConvertCollider;

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Convert")
	FVector2D ConvertSize = FVector2D(50.0f, 50.0f);

public:

	void OnConstruction(const FTransform& Transform) override;

private:

	UFUNCTION()
	void OverlapConvertBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
