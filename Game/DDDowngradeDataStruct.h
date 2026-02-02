// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Game/DDDowngradeCategories.h"
#include "DDDowngradeDataStruct.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FDDDowngradeDataStruct
{
	GENERATED_BODY()

public:

	UPROPERTY()
	UObject* Object;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UObject> Class;
	UPROPERTY(EditAnywhere)
	EDowngradeCategories Category = EDowngradeCategories::Enemy;
	UPROPERTY(EditAnywhere)
	float Amount = 0.0f;

};
