// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDProjectileManager.h"
#include "Components/BillboardComponent.h"
//My classes
#include "DragonsDefense_v2/Projectile/DDProjectile.h"
#include "DragonsDefense_v2/Game/DDGameModeBase.h"

// Sets default values
ADDProjectileManager::ADDProjectileManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ManagerIcon = CreateDefaultSubobject<UBillboardComponent>("ManagerIcon");

	RootComponent = ManagerIcon;
}

// Called when the game starts or when spawned
void ADDProjectileManager::BeginPlay()
{
	Super::BeginPlay();

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->OnGameOver.AddDynamic(this, &ADDProjectileManager::GameOverEventFunction);
		GameMode->OnGameRestart.AddDynamic(this, &ADDProjectileManager::GameOverEventFunction);
		GameMode->OnGameWon.AddDynamic(this, &ADDProjectileManager::GameOverEventFunction);
		GameMode->OnWaveOver.AddDynamic(this, &ADDProjectileManager::WaveOverEventFunction);
	}
}

void ADDProjectileManager::AddProjectileToPool(ADDProjectile* Proj)
{
	ProjectilePool.Add(Proj);
	Proj->OnProjectileDestroyed.BindUObject(this, &ADDProjectileManager::RemoveProjectileFromPool);
}

void ADDProjectileManager::RemoveProjectileFromPool(ADDProjectile* Proj)
{
	int32 ProjID = Proj->GetUniqueID();

	//Searches based on the projectile's ID
	for (ADDProjectile* SomeProj : ProjectilePool) {
		if (SomeProj && SomeProj->GetUniqueID() == ProjID) {
			SomeProj->Destroy();
			ProjectilePool.Remove(SomeProj);
			break;
		}
	}
}

void ADDProjectileManager::CleanPool()
{
	for (ADDProjectile* Proj: ProjectilePool) {
		if (Proj) {
			Proj->Destroy();
		}
	}
	ProjectilePool.Empty();
}

void ADDProjectileManager::GameOverEventFunction()
{
	CleanPool();
}
//	^^
//	||  WHAT THE FUCK IS REUSING A FUNCTION!!!
//	vv
void ADDProjectileManager::WaveOverEventFunction()
{
	CleanPool();
}
