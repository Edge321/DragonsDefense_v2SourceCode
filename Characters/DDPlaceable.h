// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/LivingActor.h"
#include "DragonsDefense_v2/Characters/DDPlaceableDataAsset.h"
#include "DragonsDefense_v2/Characters/PlaceableStates/DDPlaceableStateInterface.h"
#include "DDPlaceable.generated.h"

#define Stencil_HighlightDepth 3

class UTimelineComponent;
class UWidgetComponent;
class UDDEnemyHealthBar;
class UDDPlaceableDataAsset;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlaceableDeath, ADDPlaceable*);
DECLARE_DELEGATE_OneParam(FOnTurnEvil, ADDPlaceable*);
DECLARE_DELEGATE_OneParam(FOnTurnNormal, ADDPlaceable*);
DECLARE_DELEGATE_OneParam(FOnDamaged, ADDPlaceable*);
DECLARE_DELEGATE(FOnSold);

/**
 * 
 */
UCLASS(Abstract)
class DRAGONSDEFENSE_V2_API ADDPlaceable : public ALivingActor
{
	GENERATED_BODY()
	

public:

	ADDPlaceable();

protected:

	virtual void OnDeath() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* HealthBarWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|HealthBar")
	TSubclassOf<UDDEnemyHealthBar> HealthClass;
	//How much time in seconds does it for the delay to trigger the health bar fade animation?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|HealthBar")
	float HealthBarFadeAwayDelay = 4.0f;
	//How long in seconds does it take for the health bar to fade away?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|HealthBar")
	float HealthBarFadeAwayTime = 1.0f;
	//UNUSED
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Modifiers");
	float EasyDamageMod = 1.2f;
	//UNUSED
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Modifiers");
	float HardDamageMod = 0.9f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Modifiers");
	float EasySellingPriceMod = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Modifiers");
	float NormalSellingPriceMod = 0.8f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Modifiers");
	float HardSellingPriceMod = 0.5f;
	//How threatening is the placeable to enemies
	//Close to 1 is not threatening at all
	//Close to 100 is Defcon 1 for enemies (very threatening in case you didn't know doofus!!!)
	UPROPERTY(EditDefaultsOnly, Category = "Stats", meta = (ClampMin = "1.0", ClampMax = "100.0", UIMin = "1.0", UIMax = "100.0"));
	float ThreatLevel = 1.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats");
	FString PlaceableName = "Placeable";
	UPROPERTY(EditDefaultsOnly, Category = "Stats");
	FString PlaceableDescription = "Description";
	//The lowest price a placeable can be sold for after placement
	UPROPERTY(EditDefaultsOnly, Category = "Stats");
	int32 MinSellingPrice = 1;

	float ActualDamage;
	float ActualPrice;
	float TempHealth;
	bool bIsEvil = false;

	UPROPERTY()
	TScriptInterface<IDDPlaceableStateInterface> PlaceableState;

	virtual void ApplyDifficultyModifiers() override;

	//Pure virtuals
	virtual void OnSpawnOverlap(TArray<AActor*> OverlapActors) PURE_VIRTUAL(ADDPlaceable::OnSpawnOverlap,);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable)
	void Sell();
	UFUNCTION(BlueprintPure)
	const int32 GetSellingPrice() const;
	UFUNCTION(BlueprintPure)
	const float GetThreatLevel() const;
	UFUNCTION(BlueprintPure)
	const FString GetPlaceableName() const;
	UFUNCTION(BlueprintPure)
	const FString GetDescription() const;
	UFUNCTION(BlueprintPure)
	const bool IsEvil() const;

	//pure virtual functions
	UFUNCTION(BlueprintCallable, Category = "Highlight")
	virtual void EnableHighlight() {};
	UFUNCTION(BlueprintCallable, Category = "Highlight")
	virtual void DisableHighlight() {};
	//ConstantVisibility is for if you want the health bar to permanently be shown
	UFUNCTION(BlueprintCallable, Category = "HealthBar")
	void ShowHealthBar(const bool ConstantVisibility = false);
	UFUNCTION(BlueprintCallable, Category = "HealthBar")
	void HideHealthBar();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	UDDPlaceableDataAsset* PlaceableData;
	
	//Pure virtuals
	virtual void ConvertToEvil() PURE_VIRTUAL(ADDPlaceable::ConvertToEvil, );
	virtual const UStaticMeshComponent* GetMesh() const { return nullptr; };

	virtual void UpdateHealth(const float HealthModifier) override;
	//Use this over assigning to PlaceableState pointer. Gets all necessary jobs done
	void SetPlaceableState(IDDPlaceableStateInterface* State);
	void SetSellingPrice(const int32 Price);
	//Does not take into account game's difficulty 
	void SetSellingPriceExactly(const int32 Price);
	//Original price multiplied by mod
	void SetPriceMod(const float PriceMod);
	//Original damage multiplied by mod
	void SetDamageMod(const float DamageMod);

	FOnPlaceableDeath OnPlaceableDeath;
	FOnTurnEvil OnTurnEvil;
	FOnTurnNormal OnTurnNormal;
	FOnDamaged OnDamaged;
	FOnSold OnSold;

private:

	void InitializeHealthBar();
	void InitializeFadingAnimationVariables();

	void UpdateSellingPrice();
	void UpdateHealthBar();

	UFUNCTION()
	void FadeHealthBar(const float Value);
	UFUNCTION()
	void WaveStartEventFunction();
	/*UFUNCTION()
	void OnHealthBarFaded();*/

	TWeakObjectPtr<UDDEnemyHealthBar> HealthWidgetRef;

	float TempPrice;
	float StoredPriceMod = 1;
	int32 SellingPrice = 1;
	//If the placeable was placed during the big soul shop being present
	bool bIsPlacedPostWave = false;

	//Variables for fading away health bar
	UPROPERTY()
	UTimelineComponent* FadeTimeline;

	float TempHealthBarTime = 0.0f;
	bool bRecentlyPlaced = false;
	bool bHasBeenRecentlyAttacked = false;
	bool bManuallyShowHealthBar = false;
};
