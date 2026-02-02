// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDDebuffAreaManager.h"
#include "Components/BillboardComponent.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/DebuffArea/DDDebuffAreaBase.h"

// Sets default values
ADDDebuffAreaManager::ADDDebuffAreaManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ManagerIcon = CreateDefaultSubobject<UBillboardComponent>("Icon");

	RootComponent = ManagerIcon;
}

// Called when the game starts or when spawned
void ADDDebuffAreaManager::BeginPlay()
{
	Super::BeginPlay();
	
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->OnGameOver.AddDynamic(this, &ADDDebuffAreaManager::GameOverEventFunction);
		GameMode->OnGameRestart.AddDynamic(this, &ADDDebuffAreaManager::GameOverEventFunction);
		GameMode->OnGameWon.AddDynamic(this, &ADDDebuffAreaManager::GameOverEventFunction);
	}
}

void ADDDebuffAreaManager::AddToPool(ADDDebuffAreaBase* DebuffArea)
{
	if (DebuffArea) {
		DebuffAreaPool.Add(DebuffArea);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s unable to be added to pool"), *DebuffArea->GetName())
	}
}

void ADDDebuffAreaManager::RemoveFromPool(ADDDebuffAreaBase* DebuffArea)
{
	for (ADDDebuffAreaBase* Area : DebuffAreaPool) {
		if (Area && Area == DebuffArea) {
			Area->RemoveDebuffs();
			Area->Destroy();
			DebuffAreaPool.RemoveSwap(Area); //TODO - this shit is NOT efficient
			break;
		}
	}
}

void ADDDebuffAreaManager::CleanPool()
{
	for (ADDDebuffAreaBase* Area : DebuffAreaPool) {
		if (Area) {
			Area->RemoveDebuffs();
			Area->Destroy();
		}
	}
	//TODO - this shit is also NOT efficient
	DebuffAreaPool.Empty(); 
}

void ADDDebuffAreaManager::GameOverEventFunction()
{
	CleanPool();
}

ADDDebuffAreaBase* ADDDebuffAreaManager::SpawnDebuffArea(TSubclassOf<ADDDebuffAreaBase> DebuffAreaClass, 
	FVector Location, FRotator Rotation, bool bSpawnOnFloor)
{
	ADDDebuffAreaBase* DebuffArea = GetWorld()->SpawnActor<ADDDebuffAreaBase>(DebuffAreaClass, Location, Rotation);

	if (bSpawnOnFloor) {
		TArray<FHitResult> Hits;
		FCollisionObjectQueryParams QueryParams;
		FCollisionQueryParams Params;

		//I believe the floor is the only static object in the world
		QueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

		//Very big number to make sure to find floor
		FVector DownDirection = Location + (FVector::DownVector * 10000);

		UWorld* World = GetWorld();
		if (World) {
			World->LineTraceMultiByObjectType(Hits, Location, DownDirection, QueryParams);

			if (!Hits.IsEmpty() && Hits[0].GetActor()) {
				//Just get the first actor, most likely the floor (hopefully)
				AActor* Actor = Hits[0].GetActor();
				float ZLocation = Actor->GetActorLocation().Z;
				float DeltaZ = (ZLocation + DebuffArea->GetActualMeshSize().Z) * 0.5f;
				FVector NewLocation(Location.X, Location.Y, DeltaZ);
				DebuffArea->SetActorLocation(NewLocation);
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Floor not found, spawning DebuffArea like normal"))
			}
		}
	}

	if (DebuffArea) {
		DebuffArea->OnDeath.BindUObject(this, &ADDDebuffAreaManager::RemoveFromPool);
		AddToPool(DebuffArea);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Something went wrong with spawning a DebuffArea!"))
	}

	return DebuffArea;
}

