// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDHorizontalSpawnHelper.h"
#include "Components/BillboardComponent.h"

// Sets default values
ADDHorizontalSpawnHelper::ADDHorizontalSpawnHelper()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SpawnerHelperIcon = CreateDefaultSubobject<UBillboardComponent>("HelperIcon");

	RootComponent = SpawnerHelperIcon;
}
