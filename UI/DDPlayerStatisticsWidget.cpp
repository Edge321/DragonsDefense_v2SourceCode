// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlayerStatisticsWidget.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/UI/DDSoulShopWidget.h"
#include "DragonsDefense_v2/UI/DDUpgradeShopButton.h"
#include "DragonsDefense_v2/Characters/DDPlayer.h"

//If you want to get a string with spaces after uppercased characters for an Enum
const FString GetEnumStringWithSpacedUppercases(const TEnumAsByte<EPlayerStats> PlayerStatsEnum) 
{
	FString FinalString, TempString;
	TArray<FString> Strings;

	FString EnumString = UEnum::GetValueAsString(PlayerStatsEnum); //Returns as EPlayerStats::Stat
	FString CorrectedEnumString = EnumString.Mid(EnumString.Find("::") + 2); //+2 is to get rid of the ::

	for (int32 i = 0; i < CorrectedEnumString.Len(); i++) {
		TCHAR C = CorrectedEnumString[i];

		if (FChar::IsUpper(C) && i > 0) {
			Strings.Add(TempString);
			TempString.Empty();
			TempString.Append(" ");
		}

		TempString.AppendChar(C);
	}

	if (!TempString.IsEmpty()) {
		Strings.Add(TempString);
	}

	for (FString String : Strings) {
		FinalString.Append(String);
	}

	return FinalString;
}

const TArray<FUpgradeInfo> UDDPlayerStatisticsWidget::GetUpgradeInfoArray() const
{
	return UpgradeInfoArray;
}

void UDDPlayerStatisticsWidget::UpdateUpgradeInfoArray()
{
	if (UpgradeInfoArray.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("Attempted to update UpgradeInfoArray, but it was never populated!"))
		return;
	}

	TMap<EPlayerStats, TArray<UDDUpgradeShopButton*>> Buttons = GameModeRef->GetSoulShopWidget().GetUpgradeButtonsByCategory();
	int32 Size = Buttons.Num();
	int32 i = 0;

	for (TPair<EPlayerStats, TArray<UDDUpgradeShopButton*>> Button : Buttons) {
		UpgradeInfoArray[i].CurrentUpgrade = Button.Value[0]->GetPriceIndex();
		i++;
	}
}

void UDDPlayerStatisticsWidget::PopulateUpgradeInfoArray()
{
	if (!UpgradeInfoArray.IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("UpgradeInfoArray is already populated, skipping..."))
		return;
	}

	if (!GameModeRef.IsValid()) {
		ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GameMode) {
			GameModeRef = MakeWeakObjectPtr(GameMode);
		}
	}

	TMap<EPlayerStats, TArray<UDDUpgradeShopButton*>> Buttons = GameModeRef->GetSoulShopWidget().GetUpgradeButtonsByCategory();
	ADDPlayer& Player = GameModeRef->GetPlayer();

	for (TPair <EPlayerStats, TArray<UDDUpgradeShopButton*>> Button : Buttons) {
		UDDUpgradeShopButton* CurrentButton = Button.Value[0];
		FString PlayStatString = Player.GetPlayerStatFuzzification(EPlayerStats::MovementSpeed);

		FUpgradeInfo Info = {
			CurrentButton->GetPriceIndex(),
			CurrentButton->GetPricesSize(),
			GetEnumStringWithSpacedUppercases(Button.Key),
			Player.GetPlayerStatFuzzification(Button.Key)
		};
		UpgradeInfoArray.Add(Info);
	}
}
