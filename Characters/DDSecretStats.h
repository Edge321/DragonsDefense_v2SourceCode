// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DDSecretStats.generated.h"

/**
 * Secret stats that is for the King's calculations
 * Stats range from 0 - 100
 */
USTRUCT(BlueprintType)
struct DRAGONSDEFENSE_V2_API FDDSecretStats
{
	GENERATED_BODY()

public:
	
	//From 0 - 100
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Stats")
	int32 Offense = 0;
	//From 0 - 100
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Stats")
	int32 Defense = 0;
	//From 0 - 100
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Stats")
	int32 Support = 0;

	FDDSecretStats operator+(const FDDSecretStats& other) const {
		FDDSecretStats result;
		result.Offense = this->Offense + other.Offense;
		result.Defense = this->Defense + other.Defense;
		result.Support = this->Support + other.Support;
		return result;
	}

	FDDSecretStats& operator+=(const FDDSecretStats& other) {
		this->Offense += other.Offense;
		this->Defense += other.Defense;
		this->Support += other.Support;
		return *this;
	}

	FDDSecretStats operator-(const FDDSecretStats& other) const {
		FDDSecretStats result;
		result.Offense = this->Offense - other.Offense;
		result.Defense = this->Defense - other.Defense;
		result.Support = this->Support - other.Support;
		return result;
	}

};
