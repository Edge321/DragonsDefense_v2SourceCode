// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDDowngrade.h"

void UDDDowngrade::Initalize(FDDDowngradeStruct StructDowngrade)
{
	DowngradeData = StructDowngrade;
}

const FString UDDDowngrade::GetDowngradeName() const
{
	return DowngradeData.Name;
}

const FString UDDDowngrade::GetDescription() const
{
	return DowngradeData.Description;
}

const FString UDDDowngrade::GetRewardDescription() const
{
	return DowngradeData.RewardDescription;
}

void UDDDowngrade::InitDowngradeReward(UObject* Outer)
{
	if (!Outer) {
		UE_LOG(LogTemp, Fatal, TEXT("Passed in invalid Outer UObject"))
			return;
	}

	InitDataArrays(DowngradeData.DowngradeDataArray, Outer);
	InitDataArrays(DowngradeData.RewardDataArray, Outer);
}

void UDDDowngrade::InitDataArrays(TArray<FDDDowngradeDataStruct>& DataArray, UObject* Outer)
{
	for (FDDDowngradeDataStruct& Downgrade : DataArray) {
		UObject* DowngradeObject = NewObject<UObject>(Outer, Downgrade.Class);

		if (DowngradeObject) {
			IDDDowngradeTypeInterface* DowngradeInterface = Cast<IDDDowngradeTypeInterface>(DowngradeObject);
			if (DowngradeInterface) {
				Downgrade.Object = DowngradeObject;
			}
			else {
				UE_LOG(LogTemp, Fatal, TEXT("%s failed to implement interface"), *DowngradeObject->GetName())
			}
		}
		else {
			UE_LOG(LogTemp, Fatal, TEXT("%s failed to instantiate"), *DowngradeObject->GetName())
		}
	}
}

const TArray<FDDDowngradeDataStruct> UDDDowngrade::GetDowngradeDataArray() const
{
	return DowngradeData.DowngradeDataArray;
}

const TArray<FDDDowngradeDataStruct> UDDDowngrade::GetRewardDataArray() const
{
	return DowngradeData.RewardDataArray;
}

const int32 UDDDowngrade::GetDowngradeDuration() const
{
	return DowngradeData.DowngradeDuration;
}

const int32 UDDDowngrade::GetRewardDuration() const
{
	return DowngradeData.RewardDuration;
}

const int32 UDDDowngrade::GetUnlockedWave() const
{
	return DowngradeData.UnlockedWave;
}

const bool UDDDowngrade::GetIsSpecialCard() const
{
	return DowngradeData.bIsSpecialCard;
}

void UDDDowngrade::SetUnlockedWave(const int32 Wave)
{
	DowngradeData.UnlockedWave = Wave;
}

const bool UDDDowngrade::GetIsFinalCard() const
{
	return DowngradeData.bIsFinalCard;
}

void UDDDowngrade::ValidateClassVariables() const
{
	for (FDDDowngradeDataStruct Downgrade : DowngradeData.DowngradeDataArray) {
		check(Downgrade.Class);
	}
	for (FDDDowngradeDataStruct Reward : DowngradeData.RewardDataArray) {
		check(Reward.Class);
	}
}