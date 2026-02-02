// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDClickableManager.h"
#include "Components/BillboardComponent.h"
//My classes
#include "DragonsDefense_v2/Clickables/DDClickableBase.h"
#include "DragonsDefense_v2/Characters/DDPlayer.h"
#include "DragonsDefense_v2/Game/DDGameModeBase.h"


// Sets default values
ADDClickableManager::ADDClickableManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ManagerIcon = CreateDefaultSubobject<UBillboardComponent>("Icon");

	RootComponent = ManagerIcon;
}

// Called when the game starts or when spawned
void ADDClickableManager::BeginPlay()
{
	Super::BeginPlay();
	
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->OnGameStart.AddDynamic(this, &ADDClickableManager::GameStartEventFunction);
		GameMode->OnGameOver.AddDynamic(this, &ADDClickableManager::GameOverEventFunction);
		GameMode->OnGameRestart.AddDynamic(this, &ADDClickableManager::GameOverEventFunction);
		GameMode->OnGameWon.AddDynamic(this, &ADDClickableManager::GameOverEventFunction);
		GameMode->OnWaveOver.AddDynamic(this, &ADDClickableManager::WaveOverEventFunction);
	}
}

void ADDClickableManager::SpawnClickable(TSubclassOf<ADDClickableBase> ClickClass, FVector Location, FRotator Rotation)
{
	ADDClickableBase* Click = GetWorld()->SpawnActor<ADDClickableBase>(ClickClass, Location, Rotation);

	if (Click) {
		ClickablesPool.Add(Click);
		Click->OnFinishedClicking.BindUObject(this, &ADDClickableManager::RemoveClickableFromPool);
	}
}

void ADDClickableManager::RemoveClickableFromPool(ADDClickableBase* Clickable)
{
	for (ADDClickableBase* SomeClick : ClickablesPool) {
		if (SomeClick && SomeClick == Clickable) {
			SomeClick->Destroy();
			ClickablesPool.RemoveSwap(SomeClick);
			break;
		}
	}
}

void ADDClickableManager::CleanPool()
{
	for (ADDClickableBase* Click : ClickablesPool) {
		if (Click) {
			Click->Destroy();
		}
	}
	ClickablesPool.Empty();
}

void ADDClickableManager::HoverClickableEventFunction(ADDClickableBase* Clickable)
{
	if (Clickable) {
		Clickable->EnableHighlight();
		CurrentHoveredClickable = Clickable;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Oi! Passed in a null clickable!"))
	}
}

void ADDClickableManager::HoverOutsideClickableFunction()
{
	if (CurrentHoveredClickable.IsValid()) {
		CurrentHoveredClickable->DisableHighlight();
	}
	CurrentHoveredClickable.Reset();
}

void ADDClickableManager::GameStartEventFunction()
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->GetPlayer().OnHoverClickable.AddUniqueDynamic(this, &ADDClickableManager::HoverClickableEventFunction);
		GameMode->GetPlayer().OnHoverOutsideClickable.AddUniqueDynamic(this, &ADDClickableManager::HoverOutsideClickableFunction);
	}
}

void ADDClickableManager::GameOverEventFunction()
{
	CurrentHoveredClickable.Reset();
	CleanPool();
}

void ADDClickableManager::WaveOverEventFunction()
{
	CleanPool();
}
