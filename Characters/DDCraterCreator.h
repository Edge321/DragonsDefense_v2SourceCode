// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/Enemy.h"
#include "DDCraterCreator.generated.h"

class ADDCrater;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDCraterCreator : public AEnemy
{
	GENERATED_BODY()
	
public:

	ADDCraterCreator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* DeathCollider;

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Crater")
	TSubclassOf<ADDCrater> CraterClass;
	//This scale factor works for some reason?
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Crater")
	float ScaleFactor = 0.001f;
	//Offset for crater on the z-axis
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Crater")
	float ZOffset = 1.0f;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	void HandleTeleportation();

private:

	void AdjustDeathCollider();
	void ExpandCrater();
	void FindFloorForCrater();

	UFUNCTION()
	void DeathTouchOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	TWeakObjectPtr<ADDCrater> CraterRef;

	float TouchDamage = -9999999;
	float InitialXScale = 0.00001f;
	FVector StartPosition;
};
