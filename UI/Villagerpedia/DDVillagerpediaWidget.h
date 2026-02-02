// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDVillagerpediaWidget.generated.h"

class UVillagerpediaButton;
class UDDNewVillagerAlertWidget;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDVillagerpediaWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Villager")
	TMap<int32, bool> VillagerChecklist;
	//Total unique villagers that exist in the game
	UPROPERTY(EditDefaultsOnly, Category = "Villager")
	int32 VillagerAmount = 14;

	UPROPERTY(BlueprintReadWrite)
	TArray<UVillagerpediaButton*> ButtonArray;

	UFUNCTION(BlueprintCallable)
	void BindButtonDelegate(UVillagerpediaButton* Button);
	UFUNCTION(BlueprintCallable)
	void GetNewVillagerAlertReference();
	UFUNCTION(BlueprintImplementableEvent)
	void SetPageInfo(UVillagerpediaButton* Button);
	UFUNCTION(BlueprintCallable)
	void BindEnemySpawnerDelegate();

public:

	UFUNCTION(BlueprintCallable)
	void InitializeChecklist();
	UFUNCTION(BlueprintCallable)
	void SetPreviousWidget(UWidget* NewPreviousWidget);
	UFUNCTION(BlueprintCallable)
	void BackToPreviousWidget();
	UFUNCTION(BlueprintCallable)
	TArray<UVillagerpediaButton*> FindVillagerpediaButtons(UWidget* Widget);
	//Sees if a new enemy has been discovered using the ID
	UFUNCTION(BlueprintCallable)
	void CheckChecklist(const int32 EnemyID, const bool SendAlert = false);
	//Get a VillagerpediaButton by Enemy ID
	UFUNCTION(BlueprintCallable)
	UVillagerpediaButton* FetchVillagerpediaButton(const int32 EnemyID);

	UFUNCTION(BlueprintPure)
	const TArray<int32> GetVillagersDiscovered() const;

	void LoadFoundVillagers(TArray<int32> EnemyIDs);
	void LoadFoundVillagers(int32 EnemyID);

private:

	void SendNewVillagerAlert(const int32 EnemyID);
	void RecursiveFindButtons(UWidget* Widget, TArray<UVillagerpediaButton*>& ButtonArray);

	//Where we originally came from before opening the Villagerpedia
	UWidget* PreviousWidget;

	TWeakObjectPtr<UDDNewVillagerAlertWidget> NewVillagerRef;

};
