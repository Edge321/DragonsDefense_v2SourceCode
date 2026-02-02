// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDOccupationSquare.generated.h"

class UBoxComponent;
class ADDPlaceable;
class AEnemy;

//FVector2D is the coordinate of the square, float is the old threat level
DECLARE_DELEGATE_TwoParams(FOnThreatLevelChange, FVector2D, float);
//FVector2D is yet again the coordinate of the square
DECLARE_DELEGATE_TwoParams(FOnDetectEnemyDeath, FVector2D, AEnemy*);

UCLASS()
class DRAGONSDEFENSE_V2_API ADDOccupationSquare : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADDOccupationSquare();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* Collider;

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex);

public:

	//Sets appropriate scale for occupation square, as well as set coordinate of the square itself
	void InitializeOccupationSquare(const FVector2D Size, const FVector2D Coordinate);
	//Calculates the coordinates on the Y axis of the square on its left and right side
	void FetchSideCoordinates();
	/// <summary>
	/// Gathers up and calculates the necessary occupation square stats
	/// </summary>
	/// <param name="ThreatLevelMultiplierScaling">Scaling number grabbed from The King</param>
	void FetchOccupationStats(const float ThreatLevelMultiplierScaling);
	//Resets all occupation square stats to their default values
	void ResetOccupationSquare();
	void SetSecretStatsBases(const float Offense, const float Defense, const float Support);
	void SetSecretStatsScaling(const float Offense, const float Defense, const float Support);
	const float GetThreatLevel() const;
	const FVector2D GetSideCoordinates() const;

	FOnThreatLevelChange OnThreatLevelChange;
	FOnDetectEnemyDeath OnDetectEnemyDeath;

private:

	void AdjustSecretStatsOnPlaceable(TWeakObjectPtr<ADDPlaceable> Placeable) const;
	void OnPlaceableDeath(ADDPlaceable* Placeable);

	UFUNCTION()
	void EventFunctionDetectEnemyDeath(AEnemy* Enemy) const;

	TMap<TWeakObjectPtr<ADDPlaceable>, float> PlaceableMap;
	TMap<TWeakObjectPtr<AEnemy>, FDelegateHandle> EnemyDeathEventsMap;

	//The occupation square's left side location on the Y-axis
	float LeftSideCoordinate;
	//The occupation square's right side location on the Y-axis
	float RightSideCoordinate;

	//Scales the threat level based on how close the square is to the castle
	float ThreatLevelMultiplier = 1.0f;

	float ThreatLevel = 0.0f;
	int PlaceableAmount = 0;

	//Set by The King variables
	float OffenseBase = 0.0f;
	float OffenseScaling = 0.0f;
	float DefenseBase = 0.0f;
	float DefenseScaling = 0.0f;
	float SupportBase = 0.0f;
	float SupportScaling = 0.0f;

	FVector2D SquareCoordinate;
};
