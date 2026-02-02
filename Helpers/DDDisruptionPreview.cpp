// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDDisruptionPreview.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/Managers/DDTheKing.h"
#include "DragonsDefense_v2/Characters/DDDisrupterEnemy.h"
#include "DragonsDefense_v2/UI/DDNegativeSoulShopWidget.h"

// Sets default values
ADDDisruptionPreview::ADDDisruptionPreview()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");

	RootComponent = Mesh;

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ADDDisruptionPreview::BeginPlay()
{
	Super::BeginPlay();

	DynamicMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0);

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->OnGameStart.AddDynamic(this, &ADDDisruptionPreview::GameStartEventFunction);
		GameMode->OnGameOver.AddDynamic(this, &ADDDisruptionPreview::GameOverEventFunction);
		GameMode->OnGameRestart.AddDynamic(this, &ADDDisruptionPreview::GameOverEventFunction);
		GameMode->OnGameWon.AddDynamic(this, &ADDDisruptionPreview::GameOverEventFunction);
		GameMode->OnWaveStart.AddDynamic(this, &ADDDisruptionPreview::WaveStartEventFunction);
	}
	
	DisablePreview();
}

// Called every frame
void ADDDisruptionPreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AnimateMaterialColor();
}

void ADDDisruptionPreview::DisablePreview()
{
	Mesh->SetVisibility(false);
	DisableBlinkingEffect();
}

void ADDDisruptionPreview::EnablePreview()
{
	Mesh->SetVisibility(true);
	EnableBlinkingEffect();
}

void ADDDisruptionPreview::EnableBlinkingEffect()
{
	GetWorld()->GetTimerManager().SetTimer(BlinkingTimer, this, &ADDDisruptionPreview::UpdateOpacity, 0.05f, true, 0);
}

void ADDDisruptionPreview::DisableBlinkingEffect()
{
	GetWorld()->GetTimerManager().ClearTimer(BlinkingTimer);
}

void ADDDisruptionPreview::FetchDisruptionTransform()
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		TWeakObjectPtr<ADDDisrupterEnemy> Disrupter = GameMode->GetEnemySpawner().GetDisrupterEnemy();
		SetActorLocation(Disrupter->GetDisruptionLocation());
		FVector MeshSize = Mesh->GetStaticMesh()->GetBoundingBox().GetSize();
		SetActorScale3D(Disrupter->GetDisruptionSize() / MeshSize);
	}
}

void ADDDisruptionPreview::UpdateOpacity()
{
	CurrentOpacity = FMath::Clamp(CurrentOpacity + OpacityChange, MinOpacity, MaxOpacity);
	
	if (CurrentOpacity >= MaxOpacity || CurrentOpacity <= MinOpacity) {
		bIsIncreasing = !bIsIncreasing;
		OpacityChange *= -1;
	}
	
	if (DynamicMaterial) {
		DynamicMaterial->SetScalarParameterValue(TEXT("Opacity"), CurrentOpacity);
	}

}

void ADDDisruptionPreview::GameStartEventFunction()
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->GetTheKing().OnSpecialEnemyComing.AddUniqueDynamic(this, &ADDDisruptionPreview::SpecialEnemyIncomingEventFunction);
	}
}

void ADDDisruptionPreview::GameOverEventFunction()
{
	DisablePreview();
}

void ADDDisruptionPreview::WaveStartEventFunction()
{
	DisablePreview();
}

void ADDDisruptionPreview::SpecialEnemyIncomingEventFunction()
{
	bIsSpecialEnemyComing = true;
	FetchDisruptionTransform();
	EnablePreview();
}

