// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DragonsDefense_v2/Game/DDPlaceablePurchaseInfo.h"
#include "DragonsDefense_v2/Game/DDResettableInterface.h"
#include "DDPlaceableManager.generated.h"

class ADDPlaceablePreview;
class ADDPlaceable;
class ADDSentientPlaceable;
class ADDTrapPlaceable;
class UBillboardComponent;

enum class EDDStressMeterTypes : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKilledPlaceable, ADDPlaceable*, Placeable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlaceableTurnEvil, ADDPlaceable*, Placeable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlaceableDamaged, ADDPlaceable*, Placeable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlaceableTurnNormal, ADDPlaceable*, Placeable);
DECLARE_DELEGATE_OneParam(FOnPlaceableDespawn, EDDStressMeterTypes);
DECLARE_DELEGATE_OneParam(FOnPlaceableSpawn, EDDStressMeterTypes);
DECLARE_DELEGATE_OneParam(FOnSetPlaceableInfoVisibility, bool);

UCLASS()
class DRAGONSDEFENSE_V2_API ADDPlaceableManager : public AActor, public IDDResettableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADDPlaceableManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "PlaceablePreview")
	bool IsPreviewDisabled() const;
	UFUNCTION(BlueprintCallable, Category = "PlaceablePreview")
	FVector GetPreviewLocation() const;
	UFUNCTION(BlueprintCallable, Category = "PlaceablePreview")
	void CanPlace(bool PlaceStatus);
	//Checks if preview should be in red or green due to certain factors
	UFUNCTION(BlueprintImplementableEvent, Category = "PlaceablePreview")
	void IsPreviewValid();
	UFUNCTION(BlueprintPure, Category = "PlaceableInfo")
	bool IsBuyable() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* ManagerIcon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlaceablePreview")
	TSubclassOf<ADDPlaceablePreview> PreviewClass;
	UPROPERTY(BlueprintReadOnly, Category = "PlaceablePreview")
	ADDPlaceablePreview* Preview;
	//Array for placeables that will be spawned at the beginning of the game
	//Needed for placeable info to fetch placeables' data properly
	UPROPERTY(EditDefaultsOnly, Category = "Placeable")
	TArray<TSubclassOf<ADDPlaceable>> PlaceablesToSpawn;
	UPROPERTY(BlueprintReadOnly, Category = "PlaceablePreview")
	bool bCanPlace = true;
	UPROPERTY(BlueprintReadOnly, Category = "PlaceablePrice")
	int32 CurrentPlaceablePrice = 0;

public:

	void ResetMods() override;

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void SpawnPlaceable(TSubclassOf<ADDPlaceable> PlaceableClass, const FVector Location, const FRotator Rotation);
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void SpawnPlaceableAtCursor(TSubclassOf<ADDPlaceable> PlaceableClass);
	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void PurchasePlaceableAtCursor();
	UFUNCTION(BlueprintCallable, Category = "PlaceablePreview")
	void SetCurrentPlaceable(TSubclassOf<ADDPlaceable> PlaceableClass, int32 Price);
	UFUNCTION(BlueprintCallable, Category = "PlaceablePool")
	const TArray<ADDPlaceable*> GetPlaceablePool() const;
	//Shows all placeables' info currently on the battlefield
	UFUNCTION(BlueprintCallable, Category = "PlaceablePool")
	void ShowAllPlaceablesInfo();
	//Hides all placeables' info currently on the battlefield
	UFUNCTION(BlueprintCallable, Category = "PlaceablePool")
	void HideAllPlaceablesInfo();
	UFUNCTION(BlueprintCallable, Category = "PlaceablePool")
	const int32 GetTotalPlaceablesPlaced() const;

	const TArray<TWeakObjectPtr<ADDSentientPlaceable>>& GetSentientPool() const;
	const TArray<TWeakObjectPtr<ADDTrapPlaceable>>& GetTrapPool() const;
	//Get the placeable amount at the beginning of the wave
	const int32 GetBeginningPlaceableAmount() const;
	void SetPriceMod(const float NewPriceMod);
	void SetDamageMod(const float NewDamageMod);
	void SetAttackRadiusMod(const float NewAttackRadiusMod);
	void SetAttackSpeedMod(const float NewAttackSpeedMod);
	void SetAccuracyMod(const float NewAccuracyMod);
	//Call this after setting the appropriate mods
	void ApplyNewMods();
	//for single placeable
	void ApplyMods(ADDSentientPlaceable* Placeable);
	void ApplyMods(ADDTrapPlaceable* Placeable);


	UPROPERTY(BlueprintAssignable)
	FOnKilledPlaceable OnKilledPlaceable;
	UPROPERTY(BlueprintAssignable)
	FOnPlaceableTurnEvil OnPlaceableTurnEvil;
	UPROPERTY(BlueprintAssignable)
	FOnPlaceableDamaged OnPlaceableDamaged;
	UPROPERTY(BlueprintAssignable)
	FOnPlaceableTurnNormal OnPlaceableTurnNormal;

	FOnPlaceableDespawn OnPlaceableDespawn;
	FOnPlaceableSpawn OnPlaceableSpawn;
	FOnSetPlaceableInfoVisibility OnSetPlaceableInfoVisibility;

private:

	//Used for initializing placeable info
	void SpawnPlaceableOutsidePool(TSubclassOf<ADDPlaceable> PlaceableClass, const FVector Location, const FRotator Rotation);
	void InitializePurchaseInfo();
	void InitializeSpawnedPlaceable(ADDPlaceable* Placeable);
	void CheckPreviewValidity();
	void AddPlaceableToPool(ADDPlaceable* Placeable);
	void RemovePlaceableFromPool(ADDPlaceable* Placeable);
	void ClearPool();
	void ChangePreviewMesh(UStaticMesh* Mesh, const FVector Scale);
	//true if Sentient. False otherwise
	bool CheckPlaceableType(ADDPlaceable* Placeable);
	void BroadcastEvilPlaceable(ADDPlaceable* Placeable);
	void BroadcastDamagedPlaceable(ADDPlaceable* Placeable);
	void BroadcastNormalPlaceable(ADDPlaceable* Placeable);
	void OnPlaceableSold();

	UFUNCTION()
	void GameOverEventFunction();
	UFUNCTION()
	void WaveStartEventFunction();
	UFUNCTION()
	void WaveOverEventFunction();
	UFUNCTION()
	void SetPreviewStatus(bool IsPlacing);

	TSubclassOf<ADDPlaceable> CurrentPlaceableClass;
	TArray<ADDPlaceable*> PlaceablePool;
	TArray<TWeakObjectPtr<ADDSentientPlaceable>> SentientReferences;
	TArray<TWeakObjectPtr<ADDTrapPlaceable>> TrapReferences;

	FDDPlaceablePurchaseInfo PlaceableInfo;

	float PriceMod = 1;
	float DamageMod = 1;
	float AttackRadiusMod = 1;
	float AttackSpeedMod = 1;
	float InaccuracyMod = 0;

	int32 TotalPlaceablesPlaced = 0;
	int32 BeginningPlaceableAmount = 0;
	int32 PlaceablesSold = 0;

	bool bIsShowingAllPlaceableInfo = false;
};
