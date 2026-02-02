// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDCraterManager.h"
#include "Components/BillboardComponent.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Helpers/DDCrater.h"

// Sets default values
ADDCraterManager::ADDCraterManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ManagerIcon = CreateDefaultSubobject<UBillboardComponent>("Icon");

	RootComponent = ManagerIcon;
}

// Called when the game starts or when spawned
void ADDCraterManager::BeginPlay()
{
	Super::BeginPlay();

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->OnGameOver.AddDynamic(this, &ADDCraterManager::GameOverEventFunction);
		GameMode->OnGameRestart.AddDynamic(this, &ADDCraterManager::GameOverEventFunction);
		GameMode->OnGameWon.AddDynamic(this, &ADDCraterManager::GameOverEventFunction);
	}
}

void ADDCraterManager::AddToPool(ADDCrater* Crater)
{
	if (Crater) {
		CraterPool.Add(Crater);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s unable to be added to pool"), *Crater->GetName())
	}
}

void ADDCraterManager::RemoveFromPool(ADDCrater* Crater)
{
	for (ADDCrater* SomeCrater : CraterPool) {
		if (SomeCrater && SomeCrater == Crater) {
			SomeCrater->Destroy();
			CraterPool.RemoveSwap(Crater);
			break;
		}
	}
}

void ADDCraterManager::CleanPool()
{
	for (ADDCrater* Crater : CraterPool) {
		if (Crater) {
			Crater->Destroy();
		}
	}
	CraterPool.Empty();
}

void ADDCraterManager::GameOverEventFunction()
{
	CleanPool();
}

ADDCrater* ADDCraterManager::SpawnCrater(TSubclassOf<ADDCrater> ActorClass, FVector Location, FRotator Rotation)
{
	//Figure out spawning above floor
	ADDCrater* Crater = GetWorld()->SpawnActor<ADDCrater>(ActorClass, Location, Rotation);
	if (Crater) {
		Crater->OnLifetimeFinished.BindUObject(this, &ADDCraterManager::RemoveFromPool);
		AddToPool(Crater);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Something went wrong with spawning a crater!"))
	}

	return Crater;
}