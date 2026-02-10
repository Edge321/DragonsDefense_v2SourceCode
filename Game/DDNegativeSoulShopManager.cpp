// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDNegativeSoulShopManager.h"
#include "Components/BillboardComponent.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/DDDowngrade.h"
#include "DragonsDefense_v2/Game/DDDowngradeStruct.h"
#include "DragonsDefense_v2/Game/DDEnemySpawner.h"
#include "DragonsDefense_v2/Game/DDPlaceableManager.h"
#include "DragonsDefense_v2/Game/DDResettableInterface.h"
#include "DragonsDefense_v2/Game/Managers/DDTheKing.h"
#include "DragonsDefense_v2/Characters/DDPlayer.h"
#include "DragonsDefense_v2/UI/DDNegativeSoulShopWidget.h"

// Sets default values
ADDNegativeSoulShopManager::ADDNegativeSoulShopManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ManagerIcon = CreateDefaultSubobject<UBillboardComponent>("ManagerIcon");

	RootComponent = ManagerIcon;
}

// Called when the game starts or when spawned
void ADDNegativeSoulShopManager::BeginPlay()
{
	Super::BeginPlay();

	GameModeRef = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameModeRef) {
		GameModeRef->OnWaveOver.AddDynamic(this, &ADDNegativeSoulShopManager::WaveOverEventFunction);
		GameModeRef->OnGameOver.AddDynamic(this, &ADDNegativeSoulShopManager::GameOverEventFunction);
		GameModeRef->OnGameRestart.AddDynamic(this, &ADDNegativeSoulShopManager::GameOverEventFunction);
		GameModeRef->OnGameWon.AddDynamic(this, &ADDNegativeSoulShopManager::GameOverEventFunction);
		GameModeRef->OnGameWaveJumpChoice.AddDynamic(this, &ADDNegativeSoulShopManager::GameWaveJumpChoiceEventFunction);
	}

	ActualSpecialDowngradeWave = SpecialDowngradeWave + SpecialDowngradeWaveOffset;

	InitDowngrades();
	InitNumberPool();

	if (SpecialDowngrades.IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("No Special Downgrades were given"))
	}
	if (!FinalDowngrade) {
		UE_LOG(LogTemp, Warning, TEXT("No Final Downgrade was given"))
	}
}

const int32 ADDNegativeSoulShopManager::GetWaveDurationLeft() const
{
	return WaveDurationLeft;
}

const bool ADDNegativeSoulShopManager::GetIsDowngradeActive() const
{
	return bIsDowngradeActive;
}

const bool ADDNegativeSoulShopManager::GetIsRewardActive() const
{
	return bIsRewardActive;
}

void ADDNegativeSoulShopManager::SetNegativeShop(UDDNegativeSoulShopWidget* Widget)
{
	NegativeShopWidget = Widget;
}

void ADDNegativeSoulShopManager::Receive(const TArray<FDDDowngradeDataStruct> DataArray, const int32 Duration, const bool IsDowngrade)
{
	for (FDDDowngradeDataStruct DowngradeData : DataArray) {
		const EDowngradeCategories Category = DowngradeData.Category;
		const IDDDowngradeTypeInterface* Func = Cast<IDDDowngradeTypeInterface>(DowngradeData.Object);
		const float Amount = DowngradeData.Amount;
		check(Func)

		if (bEnableDebugger) {
			const FString String = UEnum::GetValueAsString(Category);
			UE_LOG(LogTemp, Log, TEXT("%s was modified by %f amount by %s"), *String, Amount, *Func->_getUObject()->GetName());
		}

		switch (Category) {
			case EDowngradeCategories::Player:
				CheckResettableImplementation(&(GameModeRef->GetPlayer()));
				Func->ApplyDowngrade(&(GameModeRef->GetPlayer()), Amount);
				break;
			case EDowngradeCategories::Enemy:
			case EDowngradeCategories::Wave: //EnemySpawner holds waves info
				CheckResettableImplementation(&(GameModeRef->GetEnemySpawner()));
				Func->ApplyDowngrade(&(GameModeRef->GetEnemySpawner()), Amount);
				break;
			case EDowngradeCategories::Placeables:
			case EDowngradeCategories::Sentients:
			case EDowngradeCategories::Traps:
				CheckResettableImplementation(&(GameModeRef->GetPlaceableManager()));
				Func->ApplyDowngrade(&(GameModeRef->GetPlaceableManager()), Amount);
				GameModeRef->GetPlaceableManager().ApplyNewMods();
				break;
			case EDowngradeCategories::Souls:
				Func->ApplyDowngrade(GameModeRef, Amount);
				break;
			case EDowngradeCategories::TheKing:
				Func->ApplyDowngrade(&(GameModeRef->GetTheKing()), Amount);
				break;
			default:
				UE_LOG(LogTemp, Fatal, TEXT("Invalid downgrade category used"))
					break;
		}
	}

	WaveDurationLeft = Duration;
	bIsActiveAtAll = true;
	NegativeShopWidget->SetIsDowngradeActive(IsDowngrade);
	NegativeShopWidget->SetIsRewardActive(!IsDowngrade);
}

void ADDNegativeSoulShopManager::ReceiveDowngrade(UDDDowngrade* Downgrade)
{
	Receive(Downgrade->GetDowngradeDataArray(), Downgrade->GetDowngradeDuration(), true);
}

void ADDNegativeSoulShopManager::ReceiveReward(UDDDowngrade* Downgrade)
{
	if (Downgrade->GetIsSpecialCard()) OnSpecialCardBeaten.Broadcast();
	else if (Downgrade->GetIsFinalCard()) OnFinalCardBeaten.ExecuteIfBound();

	Receive(Downgrade->GetRewardDataArray(), Downgrade->GetRewardDuration(), false);
}

void ADDNegativeSoulShopManager::InitDowngrades()
{
	for (FDDDowngradeStruct StructDowngrade : AllStructDowngrades) {
		UDDDowngrade* Downgrade = NewObject<UDDDowngrade>(this);
		if (Downgrade) {
			Downgrade->Initalize(StructDowngrade);
			Downgrade->InitDowngradeReward(this);
			if (Downgrade->GetIsSpecialCard()) {
				SpecialDowngrades.Add(Downgrade);
			}
			else if (Downgrade->GetIsFinalCard()) {
				FinalDowngrade = Downgrade;
				FinalDowngrade->SetUnlockedWave(GameModeRef->GetTheKingBossWave());
			}
			else {
				AllDowngrades.Add(Downgrade);
			}
		}
		else {
			UE_LOG(LogTemp, Fatal, TEXT("Downgrade failed to allocate"))
		}
	}
}

void ADDNegativeSoulShopManager::InitNumberPool()
{
	int32 DowngradeSize = AllDowngrades.Num();
	for (int32 i = 0; i < DowngradeSize; i++) {
		NumberPool.Add(i);
	}
	//A compact way to shuffle the array
	NumberPool.Sort([this](const int32 A, const int32 B) {
		return FMath::FRand() < 0.5f;
		});
}

void ADDNegativeSoulShopManager::ValidateNegativeShop()
{
	check(NegativeShopWidget);
}

void ADDNegativeSoulShopManager::PickDowngrades()
{
	int32 CurrentWave = GameModeRef->GetEnemySpawner().GetCurrentWave();
	TArray<UDDDowngrade*> DowngradePicks;

	if ((!SpecialDowngrades.IsEmpty() || !UsedSpecialDowngrades.IsEmpty()) && 
		(CurrentWave >= ActualSpecialDowngradeWave)) {
		ActualSpecialDowngradeWave = CurrentWave + SpecialDowngradeWave;
		PickSpecialDowngrade();
		OnSpecialCardChosen.Broadcast();
		return;
	}
	
	//This is a pick 3 type of game!!!!
	int32 PossibleDowngrades = 3;
	int32 DowngradeSize = AllDowngrades.Num();

	if (AllDowngrades.Num() < PossibleDowngrades) {
		UE_LOG(LogTemp, Fatal, TEXT("Downgrade amount not valid. Please insert 3 or more downgrades"))
	}

	TArray<int32> TempNumberPool = NumberPool;

	//Picking the 3 downgrades
	int32 i = 0;
	while (i < PossibleDowngrades) {
		if (TempNumberPool.IsEmpty()) {
			UE_LOG(LogTemp, Fatal, TEXT("Ran out of downgrades!"))
			break;
		}
		int32 Random = FMath::RandRange(0, TempNumberPool.Num() - 1);
		int32 ChosenIndex = TempNumberPool[Random];
		if (AllDowngrades[ChosenIndex]->GetUnlockedWave() <= CurrentWave) {
			DowngradePicks.Add(AllDowngrades[ChosenIndex]);
			TempNumberPool.RemoveSwap(ChosenIndex);
			i++;
		}
		else {
			TempNumberPool.RemoveSwap(ChosenIndex);
		}
	}
	//Want the final downgrade to be in the fourth card slot, which is why this if statement appears
	//at the end of this function
	if (FinalDowngrade && CurrentWave >= FinalDowngrade->GetUnlockedWave()) {
		DowngradePicks.Add(FinalDowngrade);
		OnFinalCardChosen.Broadcast();
	}

	NegativeShopWidget->SetDowngrades(DowngradePicks);
}

void ADDNegativeSoulShopManager::PickSpecialDowngrade()
{
	TArray<UDDDowngrade*> SpecialArray;

	if (SpecialDowngrades.IsEmpty()) {
		SpecialDowngrades = UsedSpecialDowngrades;
		UsedSpecialDowngrades.Empty();
	}
	//Special downgrades used only once until we went through them all
	int32 Random = FMath::RandRange(0, SpecialDowngrades.Num() - 1);
	UDDDowngrade* ChosenSpecial = SpecialDowngrades[Random];
	SpecialDowngrades.RemoveSwap(ChosenSpecial, false);
	UsedSpecialDowngrades.Add(ChosenSpecial);

	SpecialArray.Add(ChosenSpecial);

	NegativeShopWidget->SetDowngrades(SpecialArray);
}

void ADDNegativeSoulShopManager::CheckActiveDowngrade()
{
	if (bIsActiveAtAll) {
		if (--WaveDurationLeft <= 0) {
			bIsDowngradeActive = false;
			WaveDurationLeft = 0;
			for (IDDResettableInterface* ResettableActor : ActorsToReset) {
				ResettableActor->ResetMods();
			}
			ActorsToReset.Empty();
			if (bIsRewardActive) { //Finished with downgrade and reward
				bIsRewardActive = false;
				bIsActiveAtAll = false;
				NegativeShopWidget->SetIsDowngradeActive(false);
				NegativeShopWidget->SetIsRewardActive(false);
			}
			else {
				NegativeShopWidget->RequestDowngrade();
				bIsRewardActive = true;
				NegativeShopWidget->SetIsDowngradeActive(false);
				NegativeShopWidget->SetIsRewardActive(true);
			}
		}
		else {
			bIsDowngradeActive = true;
			OnDowngradeRewardDeactivated.Broadcast();
		}
	}
	else {
		OnDowngradeRewardDeactivated.Broadcast();
	}
}

void ADDNegativeSoulShopManager::CheckResettableImplementation(AActor* Actor)
{
	IDDResettableInterface* Reset = Cast<IDDResettableInterface>(Actor);
	if (Reset) {
		if (!ActorsToReset.Contains(Reset)) {
			ActorsToReset.Add(Reset);
		}
	}
	else {
		UE_LOG(LogTemp, Fatal, TEXT("Not using a resettable Actor"))
	}
}

void ADDNegativeSoulShopManager::WaveOverEventFunction()
{
	ValidateNegativeShop();
	CheckActiveDowngrade();
	if (!bIsActiveAtAll) {
		PickDowngrades();
	}
}

void ADDNegativeSoulShopManager::GameOverEventFunction()
{
	bIsRewardActive = false;
	bIsActiveAtAll = false;
	WaveDurationLeft = 0;
	NegativeShopWidget->SetIsDowngradeActive(false);
	NegativeShopWidget->SetIsRewardActive(false);
}

void ADDNegativeSoulShopManager::GameWaveJumpChoiceEventFunction() 
{
	const int32 CurrentWave = GameModeRef->GetEnemySpawner().GetCurrentWave();
	//+ 1 is to account for setting up the special downgrade wave for the next time it comes around, cus it will be skipped entirely otherwise
	const int32 Multiplication = ((CurrentWave - SpecialDowngradeWaveOffset) / SpecialDowngradeWave) + 1;
	ActualSpecialDowngradeWave = (SpecialDowngradeWave * Multiplication) + SpecialDowngradeWaveOffset;
}
