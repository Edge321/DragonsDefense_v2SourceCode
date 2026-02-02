// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDHorizontalSpawnHelper.generated.h"

class UBillboardComponent;

UCLASS()
class DRAGONSDEFENSE_V2_API ADDHorizontalSpawnHelper : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADDHorizontalSpawnHelper();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* SpawnerHelperIcon;

};
