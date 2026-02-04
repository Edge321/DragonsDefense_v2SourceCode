// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDNegativeSoulShopManager.generated.h"

class UDDNegativeSoulShopWidget;
class UBillboardComponent;
class ADDGameModeBase;
class UDDDowngrade;
class IDDResettableInterface;
struct FDDDowngradeStruct;
struct FDDDowngradeDataStruct;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDowngradeRewardDeactivated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpecialCardChosen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpecialCardBeaten);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinalCardChosen);
DECLARE_DELEGATE(FOnFinalCardBeaten);

UCLASS()
class DRAGONSDEFENSE_V2_API ADDNegativeSoulShopManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADDNegativeSoulShopManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Icon")
	UBillboardComponent* ManagerIcon;
	
	UPROPERTY(EditDefaultsOnly, Category = "Downgrades")
	TArray<FDDDowngradeStruct> AllStructDowngrades;

	//When a special downgrade is triggered every N wave
	UPROPERTY(EditDefaultsOnly, Category = "Downgrades | Special Downgrades")
	int32 SpecialDowngradeWave = 10;
	//Starting offset of the special downgrade wave, done as: SpecialDowngradeWave + SpecialDowngradeWaveOffset
	UPROPERTY(EditDefaultsOnly, Category = "Downgrades | Special Downgrades")
	int32 SpecialDowngradeWaveOffset = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool bEnableDebugger = true;


public:	

	UFUNCTION(BlueprintPure)
	const int32 GetWaveDurationLeft() const;
	UFUNCTION(BlueprintPure)
	const bool GetIsDowngradeActive() const;
	UFUNCTION(BlueprintPure)
	const bool GetIsRewardActive() const;

	void SetNegativeShop(UDDNegativeSoulShopWidget* Widget);

	void ReceiveDowngrade(UDDDowngrade* Downgrade);
	void ReceiveReward(UDDDowngrade* Downgrade);

	UPROPERTY(BlueprintAssignable)
	FOnDowngradeRewardDeactivated OnDowngradeRewardDeactivated;
	UPROPERTY(BlueprintAssignable)
	FOnSpecialCardChosen OnSpecialCardChosen;
	UPROPERTY(BlueprintAssignable)
	FOnSpecialCardBeaten OnSpecialCardBeaten;
	UPROPERTY(BlueprintAssignable)
	FOnFinalCardChosen OnFinalCardChosen;

	FOnFinalCardBeaten OnFinalCardBeaten;

private:

	void InitDowngrades();
	void InitNumberPool();
	void ValidateNegativeShop();
	void PickDowngrades();
	void PickSpecialDowngrade();
	void CheckActiveDowngrade();
	//Checks if an actor implements the resettable interface. Sets the ActorToReset variable if true
	void CheckResettableImplementation(AActor* Actor);
	void Receive(const TArray<FDDDowngradeDataStruct> DataArray, const int32 Duration, const bool IsDowngrade);

	UFUNCTION()
	void WaveOverEventFunction();
	UFUNCTION()
	void GameOverEventFunction();
	UFUNCTION()
	void GameWaveJumpChoiceEventFunction();

	ADDGameModeBase* GameModeRef;
	UDDNegativeSoulShopWidget* NegativeShopWidget;

	//UPROPERTY macro is needed or else garbage collection might mess with this array

	UPROPERTY()
	TArray<UDDDowngrade*> AllDowngrades;
	//Special downgrades reserved for the secret card in the negative soul shop
	UPROPERTY() 
	TArray<UDDDowngrade*> SpecialDowngrades;
	//Holds used up special downgrades. If all special downgrades have been taken, put them back into SpecialDowngrades
	UPROPERTY()
	TArray<UDDDowngrade*> UsedSpecialDowngrades;
	//The downgrade that ends the game if beaten. Wave Unlocked is replaced by the Game Mode's TheKingBossWave
	UPROPERTY()
	UDDDowngrade* FinalDowngrade;

	//Actors that need their stats reset after a downgrade/reward is gone
	TArray<IDDResettableInterface*> ActorsToReset;
	bool bIsActiveAtAll = false;
	bool bIsRewardActive = false;
	bool bIsDowngradeActive = false;
	int32 WaveDurationLeft = 0;

	int32 ActualSpecialDowngradeWave;

	TArray<int32> NumberPool;
};
