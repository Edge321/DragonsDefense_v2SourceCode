// Copyright (c) 2025, Edge Cope Corp. All rights reserved


#include "DDEnemySpawner.h"
#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/DDDifficulty.h"
#include "DragonsDefense_v2/Game/DDStressMeterTypes.h"
#include "DragonsDefense_v2/Game/DDNegativeSoulShopManager.h"
#include "DragonsDefense_v2/Characters/DDHorizontalEnemy.h"
#include "DragonsDefense_v2/Characters/DDDisrupterEnemy.h"
#include "DragonsDefense_v2/Characters/Bosses/DDBossEnemyBase.h"
#include "DragonsDefense_v2/Helpers/DDHorizontalSpawnHelper.h"

#include "Managers/DDSpawnRangeLimiterManager.h"

// Sets default values
ADDEnemySpawner::ADDEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SpawnerIcon = CreateDefaultSubobject<UBillboardComponent>("SpawnerIcon");

	RootComponent = SpawnerIcon;
}

// Called when the game starts or when spawned
void ADDEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->OnGameOver.AddDynamic(this, &ADDEnemySpawner::GameOverEventFunction);
		GameMode->OnGameStart.AddDynamic(this, &ADDEnemySpawner::GameStartEventFunction);
		GameMode->OnGameRestart.AddDynamic(this, &ADDEnemySpawner::GameOverEventFunction);
		GameMode->OnGameWon.AddDynamic(this, &ADDEnemySpawner::GameOverEventFunction);
		GameMode->OnWaveOver.AddDynamic(this, &ADDEnemySpawner::WaveOverEventFunction);
		GameMode->OnGameWaveJumpChoice.AddDynamic(this, &ADDEnemySpawner::WaveOverEventFunction);
		GameMode->OnWaveStart.AddDynamic(this, &ADDEnemySpawner::WaveStartEventFunction);
	}

	for (TPair<EDDEnemyIDs, TSubclassOf<AEnemy>> EnemyPair : EnemyEncyclopedia) {
		SpawnEnemyOutsidePool(EnemyPair);
	}

	Helper = FindHorizontalSpawnHelper();

	SortWaves();

	FDDWaveInfo Wave = SearchWave();
	SetWaveStats(Wave);
}

void ADDEnemySpawner::SpawnEnemyOutsidePool(TPair<EDDEnemyIDs, TSubclassOf<AEnemy>> EnemyPair) const
{
	//Get them very far away from the castle. 
	//Don't want random enemies coming out of nowhere attacking the castle after all
	//Separate the enemies by their ID * 200, makes it easy to take pictures of them
	FVector FarAwayLocation = GetActorLocation() + 10000.0f + ((int32) EnemyPair.Key * 200.0f);

	GetWorld()->SpawnActor<AEnemy>(EnemyPair.Value, FarAwayLocation, GetActorRotation());
}

void ADDEnemySpawner::SpawnEnemy()
{
	if (EnemiesToSpawn.IsEmpty()) {
		UE_LOG(LogTemp, Error, TEXT("EnemiesToSpawn array is empty! Putting in default enemy"))
		SetEnemiesToSpawn(EnemiesToSpawn); //Pass in empty array intentionally get one valid enemy
	}

	TSubclassOf<AEnemy> Enemy = SelectEnemyByRarity();


	const float TempSpawnAreaY = SpawnRangeLimiterRef->LimitEnemySpawnRange(SpawnAreaY);
	float RandomAreaY = FMath::RandRange(-TempSpawnAreaY, TempSpawnAreaY);

	FVector2D CustomRange;
	bool bHasCustomRange = Enemy.GetDefaultObject()->GetCustomSpawnRange(CustomRange);
	FVector ColliderSize = Enemy.GetDefaultObject()->GetColliderSizeWorldSpace();

	if (bHasCustomRange) {
		CustomRange.X = SpawnRangeLimiterRef->LimitEnemySpawnRange(CustomRange.X);
		CustomRange.Y = SpawnRangeLimiterRef->LimitEnemySpawnRange(CustomRange.Y);
		RandomAreaY = FMath::RandRange(CustomRange.X, CustomRange.Y);
	}
	
	FVector RandomLocation = FVector(GetActorLocation().X, RandomAreaY, GetActorLocation().Z);

	FHitResult Hit;
	bool bHit = FindFloor(Hit);

	if (bHit) {
		//Take collider into account to make the enemy above the floor
		RandomLocation.Z = Hit.Location.Z + ColliderSize.Z;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("EnemySpawner unable to find the floor!"))
	}

	AEnemy* ActualEnemy = nullptr;
	if (Enemy) {
		ActualEnemy = GetWorld()->SpawnActor<AEnemy>(Enemy, RandomLocation, GetActorRotation());
	}
	else {
		UE_LOG(LogTemp, Fatal, TEXT("Enemy class returned nullptr. Something went wrong with picking rarity or spawning the enemy"))
	}

	if (ActualEnemy) {
		if (ActualEnemy->IsA<ADDHorizontalEnemy>()) {
			FVector HelperLocation = Helper->GetActorLocation();
			//Take Helper's location on the X-axis into account
			float RandomAreaX = FMath::RandRange(-HorizontalSpawnAreaX + HelperLocation.X, HorizontalSpawnAreaX + HelperLocation.X);
			ActualEnemy->SetActorLocation(FVector(RandomAreaX, HelperLocation.Y, HelperLocation.Z));
		}

		ActualEnemy->SetAttackSightVisibility(bIsEnemyAttackBoxesVisible);
		ActualEnemy->SpawnDefaultController();
		ActualEnemy->AutoPossessAI = EAutoPossessAI::Spawned;
		ActualEnemy->OnEnemyDeath.BindUObject(this, &ADDEnemySpawner::RemoveEnemyFromPool);
		ActualEnemy->OnEnemyAttackCastle.BindUObject(this, &ADDEnemySpawner::EnemyAttacking);
		OnSpawnEnemyForID.ExecuteIfBound(ActualEnemy->GetEnemyID(), true);
		OnSpawnEnemy.ExecuteIfBound(EDDStressMeterTypes::EnemySpawn);
		ApplyMods(ActualEnemy);
		AddEnemyToPool(ActualEnemy);
		EnemyCounter++;
		if (bStrategicEnemySpawn) {
			bStrategicEnemySpawn = false;
			OnStrategicEnemySpawn.ExecuteIfBound(ActualEnemy);
		}
	}

	StartSpawn();
	CheckEnemiesSpawned();
}

AEnemy* ADDEnemySpawner::SpawnEnemy(TSubclassOf<AEnemy> EnemyClass)
{
	const float TempSpawnAreaY = SpawnRangeLimiterRef->LimitEnemySpawnRange(SpawnAreaY);
	float RandomAreaY = FMath::RandRange(-TempSpawnAreaY, TempSpawnAreaY);

	FVector2D CustomRange;
	bool bHasCustomRange = EnemyClass.GetDefaultObject()->GetCustomSpawnRange(CustomRange);
	FVector ColliderSize = EnemyClass.GetDefaultObject()->GetColliderSizeWorldSpace();

	if (bHasCustomRange) {
		CustomRange.X = SpawnRangeLimiterRef->LimitEnemySpawnRange(CustomRange.X);
		CustomRange.Y = SpawnRangeLimiterRef->LimitEnemySpawnRange(CustomRange.Y);
		RandomAreaY = FMath::RandRange(CustomRange.X, CustomRange.Y);
	}
	
	FVector RandomLocation = FVector(GetActorLocation().X, RandomAreaY, GetActorLocation().Z);

	FHitResult Hit;
	bool bHit = FindFloor(Hit);

	if (bHit) {
		RandomLocation.Z = Hit.Location.Z + ColliderSize.Z;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("EnemySpawner unable to find the floor!"))
	}

	AEnemy* ActualEnemy = GetWorld()->SpawnActor<AEnemy>(EnemyClass, RandomLocation, GetActorRotation());

	if (ActualEnemy) {
		if (ActualEnemy->IsA<ADDHorizontalEnemy>()) {
			FVector HelperLocation = Helper->GetActorLocation();
			//Take Helper's location on the X-axis into account
			float RandomAreaX = FMath::RandRange(-HorizontalSpawnAreaX + HelperLocation.X, HorizontalSpawnAreaX + HelperLocation.X);
			ActualEnemy->SetActorLocation(FVector(RandomAreaX, HelperLocation.Y, HelperLocation.Z));
		}

		ActualEnemy->SetAttackSightVisibility(bIsEnemyAttackBoxesVisible);
		ActualEnemy->SpawnDefaultController();
		ActualEnemy->AutoPossessAI = EAutoPossessAI::Spawned;
		ActualEnemy->OnEnemyDeath.BindUObject(this, &ADDEnemySpawner::RemoveEnemyFromPool);
		ActualEnemy->OnEnemyAttackCastle.BindUObject(this, &ADDEnemySpawner::EnemyAttacking);
		OnSpawnEnemyForID.ExecuteIfBound(ActualEnemy->GetEnemyID(), true);
		OnSpawnEnemy.ExecuteIfBound(EDDStressMeterTypes::EnemySpawn);
		ApplyMods(ActualEnemy);
		AddEnemyToPool(ActualEnemy);
		EnemyCounter++;
	}

	return ActualEnemy;
}

void ADDEnemySpawner::AddEnemyToPool(AEnemy* Enemy)
{
	EnemyPool.Add(Enemy);
}

void ADDEnemySpawner::CleanPool()
{
	for (AEnemy* Enemy : EnemyPool) {
		if (Enemy) {
			Enemy->Destroy();
		}
	}
	EnemyPool.Empty();
}

void ADDEnemySpawner::SortWaves()
{
	//Fucked up world to be using lambda functions
	//Uses lambda function for a custom sorting (using Priority)
	Waves.Sort([&](const FDDWaveInfo& A, const FDDWaveInfo& B) {
		return A.Priority > B.Priority;
		});
}

FDDWaveInfo ADDEnemySpawner::SearchWave() const
{
	for (FDDWaveInfo Wave : Waves) {
		if (CurrentWave % Wave.WaveTrigger == 0) {
			return Wave;
		}
	}

	//Might as well always return the bottom of the barrel
	return Waves[Waves.Num() - 1];
}

void ADDEnemySpawner::SetWaveStats(const FDDWaveInfo Wave)
{
	float LowerBoundLowSpawnInterval = 0.1f;

	/*if (!bEnemiesToSpawnModified) {
		EnemiesToSpawn = Wave.EnemiesToSpawn;
	}
	else {
		bEnemiesToSpawnModified = false;
	}*/

	EnemiesToSpawn = Wave.EnemiesToSpawn;
	MaxEnemySpawn = Wave.NumberOfEnemies * MaxEnemySpawnMod;
	HighSpawnInterval = Wave.HighSpawnInterval * HighSpawnIntervalMod;
	LowSpawnInterval = FMath::Clamp(Wave.LowSpawnInterval * LowSpawnIntervalMod, LowerBoundLowSpawnInterval, HighSpawnInterval);
}

void ADDEnemySpawner::CheckEnemiesSpawned()
{
	if (EnemyCounter >= MaxEnemySpawn) {
		StopSpawn();
	}
}

void ADDEnemySpawner::CheckIfWaveOver() const
{
	if (EnemyCounter >= MaxEnemySpawn && EnemyPool.Num() <= 0) {
		ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GameMode) {
			if (bIsFinalWave) {
				GameMode->GameWon();
			}
			else {
				GameMode->WaveOver();
			}
		}
	}
}

void ADDEnemySpawner::ApplyMods(AEnemy* Enemy) const
{
	Enemy->SetHealthMod(HealthMod);
	Enemy->SetShootCooldownMod(ShootCooldownMod);
	Enemy->SetSoulValueMod(SoulValueMod);
	Enemy->SetMovementMod(MovementMod);
	Enemy->SetDamageMod(DamageMod);
}

ADDHorizontalSpawnHelper* ADDEnemySpawner::FindHorizontalSpawnHelper() const
{
	TArray<AActor*> ActorArray;
	if (UWorld* World = GetWorld()) {
		UGameplayStatics::GetAllActorsOfClass(World, ADDHorizontalSpawnHelper::StaticClass(), ActorArray);

		check(ActorArray.Num() == 1)
		check(!ActorArray.IsEmpty());

		for (AActor* Actor : ActorArray) {
			ADDHorizontalSpawnHelper* Help = Cast<ADDHorizontalSpawnHelper>(Actor);
			if (Help) {
				return Help;
			}
		}
	}
	return nullptr;
}

void ADDEnemySpawner::EnemyAttacking() const
{
	OnEnemyAttackingCastle.ExecuteIfBound(EDDStressMeterTypes::CastleAttacked);
}

const bool ADDEnemySpawner::FindFloor(FHitResult& Hit) const
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

	TArray<AActor*> ActorsToIgnore;

	const float DownVectorMultiplication = 10000.0f;
	return UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		GetActorLocation(),
		GetActorLocation() + (FVector::DownVector * DownVectorMultiplication), //Look down from the spawner to find the floor
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		Hit,
		true);
}

void ADDEnemySpawner::BossDeathEventFunction()
{
	if (!BossEnemyRef.IsValid()) {
		UE_LOG(LogTemp, Fatal, TEXT("No boss enemy to reference!"))
		return;
	}
	
	OnBossDeath.Broadcast();

	BossEnemyRef->OnDeathBoss.Unbind();
	BossEnemyRef->OnUpdateHealthBoss.Remove(BossHealthHandler);
	BossEnemyRef.Reset();
}

void ADDEnemySpawner::UpdateHealthBossEventFunction(const float TotalHealth, const float Health) const
{
	OnUpdateBossHealth.Broadcast(TotalHealth, Health);
}

void ADDEnemySpawner::ToggleEnemyAttackBoxesEventFunction(const bool Visibility) 
{
	bIsEnemyAttackBoxesVisible = Visibility;
	for (AEnemy* Enemy : EnemyPool) {
		Enemy->SetAttackSightVisibility(Visibility);
	}
}

void ADDEnemySpawner::GameOverEventFunction()
{
	CurrentWave = 1;
	//HighSpawnInterval = OriginalSpawnInterval; //Might not be necessary tbh
	EnemyCounter = 0;
	bEnemiesToSpawnModified = false;
	bStrategicEnemySpawn = false;
	bIsFinalWave = false;
	StopSpawn();
	CleanPool();
	ResetMods();

	if (BossEnemyRef.IsValid()) {
		BossEnemyRef->OnDeathBoss.Unbind();
		BossEnemyRef->OnUpdateHealthBoss.Remove(BossHealthHandler);
		BossEnemyRef.Reset();
	}
}

void ADDEnemySpawner::GameStartEventFunction()
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->GetNegativeSoulShopManager().OnFinalCardChosen.AddUniqueDynamic(this, &ADDEnemySpawner::FinalCardChosenEventFunction);
		
		switch (GameMode->GetDifficulty()) {
		case (EDifficulty::Easy):
			StarterWaveThreshold = EasyStarterWaveThreshold;
			break;
		case (EDifficulty::Normal):
			StarterWaveThreshold = NormalStarterWaveThreshold;
			break;
		case (EDifficulty::Hard):
			StarterWaveThreshold = HardStarterWaveThreshold;
			break;
		default:
			UE_LOG(LogTemp, Error, TEXT("No difficulty found, no starter wave set"));
		}

		if (!SpawnRangeLimiterRef.IsValid()) {
			SpawnRangeLimiterRef = &(GameMode->GetSpawnRangeLimiterManager());
		}
	}

	WaveStartEventFunction();
}

void ADDEnemySpawner::WaveStartEventFunction()
{
	UE_LOG(LogTemp, Log, TEXT("Current Wave: %d"), CurrentWave);
	//The Admin would like to have a word in this function. Call him
	//StartSpawn();
}

void ADDEnemySpawner::WaveOverEventFunction()
{
	EnemyCounter = 0;
	StopSpawn();
	//CurrentWave++;
	bStrategicEnemySpawn = false;
}

void ADDEnemySpawner::FinalCardChosenEventFunction()
{
	bIsFinalWave = true;
}

void ADDEnemySpawner::RemoveEnemyFromPool(AEnemy* Enemy)
{
	int32 EnemyID = Enemy->GetUniqueID();

	for (AEnemy* SomeEnemy : EnemyPool) {
		if (SomeEnemy && SomeEnemy->GetUniqueID() == EnemyID) {
			OnDespawnEnemy.ExecuteIfBound(EDDStressMeterTypes::EnemyDeath);
			SomeEnemy->Destroy();
			EnemyPool.Remove(SomeEnemy);
			break;
		}
	}

	CheckIfWaveOver();
}

void ADDEnemySpawner::ResetMods()
{
	HealthMod = 1;
	ShootCooldownMod = 1;
	SoulValueMod = 1;
	MovementMod = 1;
	DamageMod = 1;
	MaxEnemySpawnMod = 1;
	LowSpawnIntervalMod = 1;
	HighSpawnIntervalMod = 1;
}

void ADDEnemySpawner::SpawnEnemies(const TArray<TSubclassOf<AEnemy>> Enemies, const int32 EnemyAmount, FVector SpawnLocation)
{
	if (Enemies.Num() <= 0) {
		UE_LOG(LogTemp, Warning, TEXT("Oy dumbass you passed in an empty array to EnemySpawner"))
		return;
	}

	int32 EnemyIndex;
	float RandomX;
	float RandomY;
	FVector PushForce;

	for (int32 i = 0; i < EnemyAmount; i++) {
		EnemyIndex = FMath::RandRange(0, Enemies.Num() - 1);
		//int32 RandomSignX = FMath::RandBool() ? 1 : -1;
		int32 RandomSignY = FMath::RandBool() ? 1 : -1;
		RandomX = FMath::RandRange(LowRandomXPushForce, HighRandomXPushForce);
		RandomY = FMath::RandRange(LowRandomYPushForce, HighRandomYPushForce) * RandomSignY;

		TSubclassOf<AEnemy> Enemy = Enemies[EnemyIndex];
		FVector ColliderSize = Enemy.GetDefaultObject()->GetColliderSizeWorldSpace();

		FHitResult Hit;
		bool bHit = FindFloor(Hit);

		if (bHit) {
			SpawnLocation.Z = Hit.Location.Z + ColliderSize.Z;
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("EnemySpawner unable to find the floor!"))
		}

		AEnemy* ActualEnemy = GetWorld()->SpawnActor<AEnemy>(Enemy, SpawnLocation, GetActorRotation());

		PushForce = FVector(RandomX, RandomY, 0);

		if (ActualEnemy) {
			ActualEnemy->SetAttackSightVisibility(bIsEnemyAttackBoxesVisible);
			ActualEnemy->SpawnDefaultController();
			ActualEnemy->AutoPossessAI = EAutoPossessAI::Spawned;
			ActualEnemy->OnEnemyDeath.BindUObject(this, &ADDEnemySpawner::RemoveEnemyFromPool);
			ActualEnemy->GetFloatingPawnMovement()->Velocity += PushForce;
			ActualEnemy->OnEnemyAttackCastle.BindUObject(this, &ADDEnemySpawner::EnemyAttacking);
			OnSpawnEnemyForID.ExecuteIfBound(ActualEnemy->GetEnemyID(), true);
			OnSpawnEnemy.ExecuteIfBound(EDDStressMeterTypes::EnemySpawn);
			ApplyMods(ActualEnemy);
			AddEnemyToPool(ActualEnemy);
		}
	}
}

void ADDEnemySpawner::UsePredefinedWaves()
{
	FDDWaveInfo Wave = SearchWave();
	SetWaveStats(Wave);
	PreSpawnSpecialEventEnemies();
}

const int32 ADDEnemySpawner::GetCurrentWave() const
{
	return CurrentWave;
}

const int32 ADDEnemySpawner::GetEnemiesLeftToSpawn() const
{
	return MaxEnemySpawn - EnemyCounter;
}

void ADDEnemySpawner::SetEnemiesToSpawn(const TArray<TSubclassOf<AEnemy>> NewEnemiesToSpawn)
{
	if (NewEnemiesToSpawn.IsEmpty()) {
		EnemiesToSpawn = { EnemyEncyclopedia[EDDEnemyIDs::NORMAL_ENEMY_ONE] };
	}
	else {
		EnemiesToSpawn = NewEnemiesToSpawn;
	}

	bEnemiesToSpawnModified = true;
}

void ADDEnemySpawner::SetMaxEnemySpawn(const int32 NewMaxEnemySpawn)
{
	MaxEnemySpawn = NewMaxEnemySpawn;
}

void ADDEnemySpawner::SetSpawnIntervals(const float NewLowSpawnInterval, const float NewHighSpawnInterval)
{
	float LowerBoundLowSpawnInterval = 0.1f;
	
	HighSpawnInterval = NewHighSpawnInterval;
	LowSpawnInterval = FMath::Clamp(NewLowSpawnInterval, LowerBoundLowSpawnInterval, HighSpawnInterval);
}

void ADDEnemySpawner::SetLowSpawnInterval(const float NewLowSpawnInterval)
{
	float LowerBoundLowSpawnInterval = 0.1f;
	LowSpawnInterval = FMath::Clamp(NewLowSpawnInterval, LowerBoundLowSpawnInterval, HighSpawnInterval);
}

void ADDEnemySpawner::SetHighSpawnInterval(const float NewHighSpawnInterval)
{
	HighSpawnInterval = NewHighSpawnInterval;
}

void ADDEnemySpawner::SetHealthMod(const float NewHealthMod)
{
	HealthMod = NewHealthMod;
}

void ADDEnemySpawner::SetShootCooldownMod(const float NewShootCooldownMod)
{
	ShootCooldownMod = NewShootCooldownMod;
}

void ADDEnemySpawner::SetSoulValueMod(const float NewSoulValueMod)
{
	SoulValueMod = NewSoulValueMod;
}

void ADDEnemySpawner::SetMovementMod(const float NewMovementMod)
{
	MovementMod = NewMovementMod;
}

void ADDEnemySpawner::SetDamageMod(const float NewDamageMod)
{
	DamageMod = NewDamageMod;
}

void ADDEnemySpawner::SetMaxEnemySpawnMod(const float NewMaxEnemySpawnMod)
{
	MaxEnemySpawnMod = NewMaxEnemySpawnMod;
}

void ADDEnemySpawner::SetLowSpawnIntervalMod(const float NewLowSpawnIntervalMod)
{
	LowSpawnIntervalMod = NewLowSpawnIntervalMod;
}

void ADDEnemySpawner::SetHighSpawnIntervalMod(const float NewHighSpawnIntervalMod)
{
	HighSpawnIntervalMod = NewHighSpawnIntervalMod;
}

void ADDEnemySpawner::SetCurrentWaveInfo(const FDDWaveInfo Wave)
{
	SetWaveStats(Wave);
}

void ADDEnemySpawner::SetCurrentWave(const int32 Wave)
{
	CurrentWave = Wave;
}

void ADDEnemySpawner::SetStrategicSpawnFlag()
{
	bStrategicEnemySpawn = true;
}

void ADDEnemySpawner::SetEnemySpawnRange(FVector2D Range)
{
	SpawnRange = Range;
}

TArray<TSubclassOf<AEnemy>> ADDEnemySpawner::GetEnemiesToSpawn() const
{
	return EnemiesToSpawn;
}

int32 ADDEnemySpawner::GetMaxEnemySpawn() const
{
	return MaxEnemySpawn;
}

int32 ADDEnemySpawner::GetStarterWaveThreshold() const
{
	return StarterWaveThreshold;
}

float ADDEnemySpawner::GetLowSpawnInterval() const
{
	return LowSpawnInterval;
}

float ADDEnemySpawner::GetHighSpawnInterval() const
{
	return HighSpawnInterval;
}

TWeakObjectPtr<ADDDisrupterEnemy> ADDEnemySpawner::GetDisrupterEnemy() const
{
	return DisrupterRef;
}

TMap<EDDEnemyIDs, TSubclassOf<AEnemy>> ADDEnemySpawner::GetEnemyEncyclopedia() const
{
	return EnemyEncyclopedia;
}

void ADDEnemySpawner::PreSpawnSpecialEventEnemies()
{
	TArray <TSubclassOf<AEnemy>> SpecialEnemyClassesToRemove;

	//These special enemy should be spawned only once, here
	for (TSubclassOf<AEnemy> EnemyClass : EnemiesToSpawn) {
		if (EnemyClass->IsChildOf(ADDDisrupterEnemy::StaticClass())) {
			ADDDisrupterEnemy* Disrupter = Cast<ADDDisrupterEnemy>(SpawnEnemy(EnemyClass));
			DisrupterRef = MakeWeakObjectPtr(Disrupter);
			SpecialEnemyClassesToRemove.Add(EnemyClass);
		}
	}

	for (TSubclassOf<AEnemy> EnemyClass : SpecialEnemyClassesToRemove) {
		EnemiesToSpawn.RemoveSwap(EnemyClass);
	}
}

void ADDEnemySpawner::PreSpawnBossEnemies()
{
	TArray <TSubclassOf<AEnemy>> BossEnemyClassesToRemove;

	//These special enemy should be spawned only once, here
	for (TSubclassOf<AEnemy> EnemyClass : EnemiesToSpawn) {
		if (EnemyClass->IsChildOf(ADDBossEnemyBase::StaticClass())) {
			ADDBossEnemyBase* Boss = Cast<ADDBossEnemyBase>(SpawnEnemy(EnemyClass));
			OnBossSpawn.Broadcast(Boss);
			Boss->OnDeathBoss.BindUObject(this, &ADDEnemySpawner::BossDeathEventFunction);
			BossHealthHandler = Boss->OnUpdateHealthBoss.AddUObject(this, &ADDEnemySpawner::UpdateHealthBossEventFunction);
			BossEnemyRef = MakeWeakObjectPtr(Boss);
			BossEnemyClassesToRemove.Add(EnemyClass);
		}
	}
	
	for (TSubclassOf<AEnemy> EnemyClass : BossEnemyClassesToRemove) {
		EnemiesToSpawn.RemoveSwap(EnemyClass);
	}
}

TSubclassOf<AEnemy> ADDEnemySpawner::SelectEnemyByRarity()
{
	float TotalWeight = 0.0f;
	for (TSubclassOf<AEnemy> EnemyClass : EnemiesToSpawn) {
		//The inversion of rarity
		TotalWeight += 1.0f / EnemyClass.GetDefaultObject()->GetRarity();
	}

	float RandomWeight = FMath::RandRange(0.0f, TotalWeight);
	float CurrentWeight = 0.0f;

	for (TSubclassOf<AEnemy> EnemyClass : EnemiesToSpawn) {
		CurrentWeight += 1.0f / EnemyClass.GetDefaultObject()->GetRarity();
		if (CurrentWeight >= RandomWeight) {
			return EnemyClass;
		}
	}

	return nullptr;
}

void ADDEnemySpawner::StartSpawn()
{
	float RandomInterval = FMath::RandRange(LowSpawnInterval, HighSpawnInterval);
	//Spawn enemy will call this function again
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &ADDEnemySpawner::SpawnEnemy, RandomInterval, true);
}

void ADDEnemySpawner::StopSpawn()
{
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
}

