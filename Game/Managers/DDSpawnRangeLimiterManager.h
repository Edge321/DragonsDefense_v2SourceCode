// Copyright (c) 2025, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDSpawnRangeLimiterManager.generated.h"

class UTimelineComponent;
class UBillboardComponent;
class ADDEnemySpawner;

UCLASS()
class DRAGONSDEFENSE_V2_API ADDSpawnRangeLimiterManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADDSpawnRangeLimiterManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* SpawnRangeIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Limit | Class")
	TSubclassOf<UStaticMeshComponent> RedLineLimiterClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Limit | Class")
	TSubclassOf<UStaticMeshComponent> SpawnPointerClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Limit")
	FVector2D StartingSpawnRangeLimit = FVector2D(-500.0f, 500.0f);
	//How much does easy difficulty affect the starting spawn range? Is multiplied with the starting spawn range
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Limit", meta = (ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0"))
	float EasySpawnRangeLimitModifier = 0.8f;
	//How much does hard difficulty affect the starting spawn range? Is multiplied with the starting spawn range
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Limit", meta = (ClampMin = "0.0", ClampMax = "2.0", UIMin = "0.0", UIMax = "2.0"))
	float HardSpawnRangeLimitModifier = 1.4f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Limit")
	FVector2D SpawnRangeMax = FVector2D(-2900.0f, 2900.0f);
	//How much does both sides of the spawn range limit increase by every N waves? (N is defined by SpawnRangeIncreaseWave)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Limit")
	float SpawnRangeIncrease = 250.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Limit")
	int32 SpawnRangeIncreaseWave = 2;
	//How much spacing is needed to add more spawn pointer arrows? Looks at Y-axis 0, 
	//and goes from there on both positive and negative sides
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Limit | Spawn Pointers")
	float ArrowSpacing = 500.0f;
	//How much space should be put between the spawn pointers and the red lines? 
	//This is considered in the Arrow Spacing calculation
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Limit | Spawn Pointers")
	float ArrowSpacingBetweenRedLines = 250.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Limit | Spawn Pointers")
	FVector ArrowScale = FVector(5.0f, 5.0f, 5.0f);
	//How much should arrows be offset on the x-axis relative to the location of the manager
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Limit | Spawn Pointers")
	float ArrowXOffset = 250.0f;
	//How much should arrows be offset on the z-axis relative to the location of the manager
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Limit | Spawn Pointers")
	float ArrowZOffset = 250.0f;
	//How fast the arrow is moving in the sine wave
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Limit | Spawn Pointers | Animation")
	float ArrowSpeed = 2.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Limit | Spawn Pointers | Animation")
	float ArrowAmplitude = 50.0f;
	//How much time in seconds does it take for the spawn pointers to fade away
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Limit | Spawn Pointers | Animation")
	float FadingTime = 2.0f;
	//How much time in seconds does it take until the spawn pointers fading animation starts
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Limit | Spawn Pointers | Animation")
	float FadingDelay = 5.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Limit | Red Line")
	float RedLineLength = 500.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Limit | Red Line")
	float TopRedLinesLength = 1500.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn Limit | Red Line")
	float TopRedLinesOffsetY = -500.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetTemporarySpawnRange(const FVector2D SpawnRange);

	const FVector2D GetCurrentSpawnRangeLimit() const;

	//Returns a float that limits the enemy's Y-axis spawn
	float LimitEnemySpawnRange(const float SpawnYAxis) const;

private:

	void InitializeRedLineLimiters();
	void InitializeSpawnPointers();
	void InitializeFadingAnimationVariables();
	//Shows all spawn pointers between the spawn range limit
	void ShowSpawnPointersWithinLimit();
	void HideAllSpawnPointersInstantly();
	void HideAllSpawnPointersFading();
	void IncreaseSpawnRange();

	UFUNCTION()
	void FadeSpawnPointers(const float Value);

	UFUNCTION()
	void GameStartEventFunction();
	UFUNCTION()
	void GameOverEventFunction();
	UFUNCTION()
	void WaveStartEventFunction();
	UFUNCTION()
	void WaveOverEventFunction();
	UFUNCTION()
	void OnSpawnPointersInvisible();
	UFUNCTION()
	void GameWaveJumpChoiceEventFunction();

	TArray<FVector> SpawnPointerLocations;
	UPROPERTY()
	TArray<UStaticMeshComponent*> SpawnPointers;
	UPROPERTY()
	UStaticMeshComponent* LeftRedLineLimiter;
	UPROPERTY()
	UStaticMeshComponent* TopLeftRedLineLimiter;
	UPROPERTY()
	UStaticMeshComponent* RightRedLineLimiter;
	UPROPERTY()
	UStaticMeshComponent* TopRightRedLineLimiter;

	TWeakObjectPtr<ADDEnemySpawner> EnemySpawnerRef;

	FVector2D CurrentSpawnRangeLimit;
	FVector2D TempSpawnRangeLimit;

	bool bTempSpawnRangeLimitActive = false;
	bool bAllArrowsHidden = false;

	float TotalTime = 0.0f;
	
	//Fading animation variables
	UPROPERTY()
	UTimelineComponent* FadeTimeline;

	TArray<TArray<TWeakObjectPtr<UMaterialInstanceDynamic>>> SpawnPointerDynamicMaterials;

	FTimerHandle FadeTimerHandle;

	float LastTimeFadingPointers = 0.0f;
};
