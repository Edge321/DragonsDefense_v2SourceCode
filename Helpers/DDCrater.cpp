// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDCrater.h"
#include "Components/BoxComponent.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"

// Sets default values
ADDCrater::ADDCrater()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Collider = CreateDefaultSubobject<UBoxComponent>("Collider");

	RootComponent = Mesh;
	Collider->SetupAttachment(Mesh);

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Collider->SetCollisionObjectType(ECC_EnemyChannel);
	DDColliderLibrary::SetCollisionChannelToIgnore(Collider, ECC_WorldDynamic);

}

// Called when the game starts or when spawned
void ADDCrater::BeginPlay()
{
	Super::BeginPlay();

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->OnWaveOver.AddDynamic(this, &ADDCrater::WaveOverEventFunction);
	}
	
}

const FVector ADDCrater::GetMeshSize() const
{
	return Mesh->GetStaticMesh()->GetBoundingBox().GetSize();
}

void ADDCrater::CheckWaveLifeTime()
{
	if (--WaveLifetime <= 0) {
		OnLifetimeFinished.ExecuteIfBound(this);
	}
}

void ADDCrater::WaveOverEventFunction()
{
	CheckWaveLifeTime();
}

