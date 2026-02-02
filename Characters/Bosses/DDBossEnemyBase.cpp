// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDBossEnemyBase.h"
#include "Components/BillboardComponent.h"
#include "Components/WidgetComponent.h"
//My classes
#include "DragonsDefense_v2/Game/DDBossAIBase.h"
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/DDEnemySpawner.h"
#include "DragonsDefense_v2/Game/DDDifficulty.h"
#include "DragonsDefense_v2/Component/BossMageMinionComponent.h"
#include "DragonsDefense_v2/Projectile/DDSmartDebuffAreaProjectile.h"
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"

ADDBossEnemyBase::ADDBossEnemyBase()
{
	//NOTE - Mesh needs to be moved up enough such that it does not fail to spawn
	//Needed to make the mesh moveable since root components aren't allowed to be moved
	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>("Scene");
	Mesh->SetupAttachment(SceneComponent);
	RootComponent = SceneComponent;

	BossIconBillboard = CreateDefaultSubobject<UBillboardComponent>("BossIcon");

	BossIconBillboard->SetupAttachment(Mesh);

	BossIconBillboard->bHiddenInGame = false;
	BossIconBillboard->SetWorldScale3D(FVector(2.5f, 2.5f, 2.5f));
}

void ADDBossEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());

	HealthBarWidget->bHiddenInGame = true;

	BossAI = NewObject<UDDBossAIBase>(this, BossAIClass);

	float HealthDifficultyMultiplier = 1.0f;

	switch (GameMode->GetDifficulty()) {
		case EDifficulty::Easy:
			HealthDifficultyMultiplier = EasyHealthMod;
			break;
		case EDifficulty::Normal:
			//Do nothing
			break;
		case EDifficulty::Hard:
			HealthDifficultyMultiplier = HardHealthMod;
			break;
	}

	ADDEnemySpawner* Spawner = &GameMode->GetEnemySpawner();

	if (Spawner) {
		TempHealth = (FMathf::Pow(Spawner->GetCurrentWave(), HealthExponentScaling) + TempHealth) * HealthDifficultyMultiplier;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("%s unable to set boss health appropiately"), *GetName())
	}
	TotalHealth = TempHealth;

	//Searching and storing Minion components
	TSet<UActorComponent*> Components = GetComponents();
	for (UActorComponent* Component : Components) {
		UBossMageMinionComponent* MinionComponent = Cast<UBossMageMinionComponent>(Component);
		if (MinionComponent) {
			Minions.Add(MinionComponent);
		}
	}

	//Choose a random skin to put on if there exists a list
	if (!BossSkins.IsEmpty()) {
		int32 RandomIndex = FMath::RandRange(0, BossSkins.Num() - 1);
		Mesh->SetStaticMesh(BossSkins[RandomIndex]);
	}
}

void ADDBossEnemyBase::Shoot()
{
	OnEnemyAttackCastle.ExecuteIfBound();

	ShootProjectile();
}

void ADDBossEnemyBase::ShootPlaceable()
{
	if (PlaceablesInSight.Num() <= 0) {
		UE_LOG(LogTemp, Error, TEXT("There are no placeables in sight! Stopping shooting..."))
		StopShooting();
		return;
	}

	ShootProjectile();
}

void ADDBossEnemyBase::OnDeath()
{
	BossAI->DoBossDeath(this);
	OnDeathBoss.ExecuteIfBound();
	Super::OnDeath();
}

void ADDBossEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	BossAI->DoBossAI(this);
}

void ADDBossEnemyBase::UpdateHealth(const float HealthModifier)
{
	Super::UpdateHealth(HealthModifier);

	OnUpdateHealthBoss.Broadcast(TotalHealth, TempHealth);
}

void ADDBossEnemyBase::DisableMinionComponent(const int32 Index)
{
	if (Minions.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("No more minion components to disable! Skipping..."))
		return;
	}

	int32 ActualIndex = FMath::Clamp(Index, -1, Minions.Num() - 1);
	if (ActualIndex < 0) {
		ActualIndex = FMath::RandRange(0, Minions.Num() - 1);
	}

	UBossMageMinionComponent* MinionComponent = Minions[ActualIndex];
	MinionComponent->DisableMinion();
	Minions.RemoveSwap(MinionComponent);
	MinionComponent->DestroyComponent();
}

void ADDBossEnemyBase::DisableAllMinionComponents()
{
	for (UBossMageMinionComponent* MinionComponent : Minions) {
		MinionComponent->DisableMinion();
		MinionComponent->DestroyComponent();
	}
	Minions.Empty();
}

const TSubclassOf<AEnemy> ADDBossEnemyBase::GetEnemyClassSpawn() const
{
	return EnemyClassSpawn;
}

const int32 ADDBossEnemyBase::GetEnemySpawnAmount() const
{
	return EnemySpawnAmount;
}

const float ADDBossEnemyBase::GetTotalHealth() const
{
	return TotalHealth;
}

const float ADDBossEnemyBase::GetDamagedStateDebuffAmount() const
{
	return DamagedStateDebuffAmount;
}

const float ADDBossEnemyBase::GetInjuredStateMovement() const
{
	return InjuredStateMovement;
}

const float ADDBossEnemyBase::GetNearDeathStateMovement() const
{
	return NearDeathStateMovement;
}

const int32 ADDBossEnemyBase::GetSummonSoulAmount() const
{
	return SummonSoulAmount;
}

void ADDBossEnemyBase::ShootProjectile() const
{
	ADDProjectile* Proj = GetWorld()->SpawnActor<ADDProjectile>(Projectile, GetActorLocation() + ProjectileOffset, GetActorRotation());

	if (Proj) {
		Proj->SetProjectileOwner(this);
		Proj->SetVelocity(BossProjectileMovement);
		Proj->SetCollisionChannelToIgnore(ECC_EnemyChannel);
		Proj->SetCollisionChannelToIgnore(ECC_AttackRadiusChannel);
		Proj->SetCollisionChannelToIgnore(ECC_EnemyRadiusChannel);
		if (Proj->IsA<ADDSmartDebuffAreaProjectile>()) {
			ADDSmartDebuffAreaProjectile* SmartProj = Cast<ADDSmartDebuffAreaProjectile>(Proj);
			SmartProj->SetPlayerDamage(Damage);
		}
		Proj->SetDamage(BossProjectileDamage);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("ERROR: Enemy projectile unable to spawn"))
	}
}
