// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
//My classes
#include "DragonsDefense_v2/Characters/DDSentientPlaceable.h"
#include "DragonsDefense_v2/Characters/DDTrapPlaceable.h"
#include "DragonsDefense_v2/Characters/DDEnemyDataAsset.h"
#include "DragonsDefense_v2/Projectile/DDProjectile.h"
#include "DragonsDefense_v2/Characters/DDPlayer.h"
#include "DragonsDefense_v2/Characters/DDEnemyTypes.h"
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/DDDifficulty.h"
#include "DragonsDefense_v2/UI/DDEnemyHealthBar.h"
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"

// Sets default values
AEnemy::AEnemy()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>("FloatingPawnMovement");
	AttackSight = CreateDefaultSubobject<UBoxComponent>("AttackSight");
	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>("HealthBar");

	RootComponent = Mesh;
	Collider->SetupAttachment(Mesh);
	Arrow->SetupAttachment(Mesh);
	Arrow->SetWorldRotation(FQuat(0, 0, 180, 0));
	AttackSight->SetupAttachment(Mesh);
	HealthBarWidget->SetupAttachment(Mesh);
	HealthBarWidget->SetWorldRotation(FQuat(0, 0, 180, 0));
	
	//Forces only the collider to have collision. 
	//Whether this is a good idea, I dont know
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Collider->SetCollisionObjectType(ECC_EnemyChannel);
	AttackSight->SetCollisionObjectType(ECC_AttackRadiusChannel);
	HealthBarWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DDColliderLibrary::SetCollisionChannelToIgnore(AttackSight, ECC_AttackRadiusChannel);
	AttackSight->ComponentTags.Add(ENEMY_COLLIDER_TAG);

	AttackSight->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OverlapBegin);
	AttackSight->OnComponentEndOverlap.AddDynamic(this, &AEnemy::OverlapEnd);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	//Prevents enemy from accelerating like crazy at the beginning
	FloatingPawnMovement->MaxSpeed = MovementSpeed;
	TempHealth = Health;

	if (!bEnableAttackSentients && !bEnableAttackTraps) {
		AttackSight->bHiddenInGame = true; //Hide it if no attacking placeables, no point to it
	}

	ValidateProjectile();
	FindPlayer();
	ApplyDifficultyModifiers();

	HealthBarWidget->SetCastShadow(false);
	HealthBarWidget->SetWorldScale3D(FVector(1, 1, 1));

	//Cache health widget pointer
	UDDEnemyHealthBar* HealthWidget = Cast<UDDEnemyHealthBar>(HealthBarWidget->GetWidget());
	if (HealthWidget) {
		HealthWidgetRef = MakeWeakObjectPtr(HealthWidget);
		HealthWidgetRef->SetHealthScale(Health);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Health bar widget class not set for %s!"), *GetName())
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsShooting) {
		const FVector ActualMovement = FVector(MovementSpeed * -1.0, 0, 0);
		FloatingPawnMovement->AddInputVector(ActualMovement);
	}
	
	if (CheckDistance()) {
		StopMoving();
		StartShooting();
	}

	AdjustHealthBarRotation();
}

void AEnemy::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (HealthClass) {
		HealthBarWidget->SetWidgetClass(HealthClass);
	}

	HealthBarWidget->SetWorldScale3D(FVector(1, 1, 1));

	if (!bOverrideAttackSightColliderSize) {
		AdjustAttackBox();
	}

	if (EnemyData) {
		EnemyData->EnemyID = EnemyID;
		EnemyData->EnemyName = EnemyName;
		EnemyData->EnemyDescription = EnemyDescription;
		EnemyData->Damage = Damage;
		EnemyData->Health = Health;
		EnemyData->AttackSpeed = ShootCooldown;
		EnemyData->MovementSpeed = MovementSpeed;
		EnemyData->SoulValue = SoulValue;
		EnemyData->SpecialAbilityDescription = SpecialAbilityDescription;
	}
}

UStaticMeshComponent* AEnemy::GetMeshComponent() const
{
	return Mesh;
}

UFloatingPawnMovement* AEnemy::GetFloatingPawnMovement() const
{
	return FloatingPawnMovement;
}

FVector AEnemy::GetColliderSizeWorldSpace() const
{
	FVector MeshScale = Mesh->GetRelativeScale3D();
	return Collider->GetUnscaledBoxExtent() * Collider->GetRelativeScale3D() * MeshScale;
}

const bool AEnemy::GetCustomSpawnRange(FVector2D& CustomRange) const
{
	CustomRange = CustomSpawnRange;
	bool bHasCustomRange = !CustomSpawnRange.IsZero();
	return bHasCustomRange;
}

const float AEnemy::GetMovementSpeed() const
{
	return MovementSpeed;
}

const float AEnemy::GetDistanceFromCastle() const 
{
	return DistanceFromCastle;
}

const float AEnemy::GetMaxSpeed() const
{
	return FloatingPawnMovement->GetMaxSpeed();
}

const float AEnemy::GetRarity() const
{
	return Rarity;
}

const float AEnemy::GetProjectileSpeed() const
{
	return ProjectileSpeed;
}

void AEnemy::SetMovementSpeed(const float NewMovementSpeed)
{
	MovementSpeed = NewMovementSpeed;
	FloatingPawnMovement->MaxSpeed = FMathf::Abs(MovementSpeed);
}

const bool AEnemy::GetIsSentientDetectable() const
{
	return bIsSentientDetectable;
}

const bool AEnemy::GetIsTrapDetectable() const
{
	return bIsTrapDetectable;
}

const float AEnemy::GetSlowDebuffResistance() const 
{
	return SlowDebuffResistance;
}

const int32 AEnemy::GetEnemyID() const
{
	return EnemyID;
}

const EDDEnemyType AEnemy::GetEnemyType() const
{
	return Type;
}

const bool AEnemy::GetDestroysPlaceables() const 
{
	return bDestroysPlaceables;
}

bool AEnemy::CheckDistance()
{
	//Go back if we never found the player. 
	//Pretty much for the enemies that are spawned at the beginning for initialization
	if (!Player) return false;

	//Distance formula on one dimension
	DistanceFromCastle = FMath::Abs(GetActorLocation().X - Player->GetActorLocation().X);

	if (DistanceFromCastle < DistanceToAttack) {
		return true;
	}	
	else {
		return false;
	}
}

void AEnemy::ApplyDifficultyModifiers()
{
	EDifficulty Difficulty = EDifficulty::Normal;

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		Difficulty = GameMode->GetDifficulty();
	}

	switch (Difficulty) {
		case(EDifficulty::Easy):
			Damage *= EasyDamageMod;
			Health *= EasyHealthMod;
			TempHealth = Health;
			MovementSpeed *= EasyMovementSpeedMod;
			ShootCooldown *= EasyShootCooldownMod;
			SoulValue = FMath::Clamp(SoulValue + EasySoulValueMod, 1, MAX_int32);
			break;
		case(EDifficulty::Normal):
			//Nothing changes
			break;
		case(EDifficulty::Hard):
			Damage *= HardDamageMod;
			Health *= HardHealthMod;
			TempHealth = Health;
			MovementSpeed *= HardMovementSpeedMod;
			ShootCooldown *= HardShootCooldownMod;
			//SoulValue = FMath::Clamp(SoulValue + HardSoulValueMod, 1, MAX_int32);
			break;
		default:
			UE_LOG(LogTemp, Fatal, TEXT("No difficulty exist from GameMode somehow? What the hell is this"))
			break;
	}

	SetMovementSpeed(MovementSpeed); //Update movement speed correctly
}

void AEnemy::UpdateHealth(const float HealthModifier)
{
	TempHealth += HealthModifier;

	if (TempHealth <= 0) {
		OnDeath();
	}

	UpdateHealthBar();
}

void AEnemy::SetHealthMod(const float HealthMod)
{
	Health *= HealthMod;
}

void AEnemy::SetShootCooldownMod(const float ShootCooldownMod)
{
	ShootCooldown *= ShootCooldownMod;
}

void AEnemy::SetSoulValueMod(const float SoulValueMod)
{
	SoulValue *= SoulValueMod;
}

void AEnemy::SetMovementMod(const float MovementMod)
{
	MovementSpeed *= MovementMod;
}

void AEnemy::SetDamageMod(const float DamageMod)
{
	Damage *= DamageMod;
}

void AEnemy::SetAttackSightVisibility(const bool Visibility) 
{
	if (bEnableAttackSentients || bEnableAttackTraps) {
		AttackSight->SetVisibility(Visibility);
	}
	//TODO - add collider or some indicator for distance to attack castle
}

void AEnemy::ForceAttack(ADDPlaceable* Placeable)
{
	if (Placeable) {
		PlaceablesInSight.Add(Placeable);
		StartShootingPlaceable();
		StopMoving();
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Invalid placeable passed in, ignoring"))
	}
}

void AEnemy::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;
	if (OtherActor->IsA<ADDPlaceable>() && Cast<ADDPlaceable>(OtherActor)->IsEvil()) return;

	const bool bAddPlaceable = (bEnableAttackSentients && OtherActor->IsA<ADDSentientPlaceable>()) ||
								(bEnableAttackTraps && OtherActor->IsA<ADDTrapPlaceable>());

	if (bAddPlaceable) {
		PlaceablesInSight.Add(Cast<ADDPlaceable>(OtherActor));
		StartShootingPlaceable();
		StopMoving();
	}
}

void AEnemy::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA<ADDPlaceable>()) {
		ADDPlaceable* Place = Cast<ADDPlaceable>(OtherActor);
		PlaceablesInSight.RemoveSwap(Place);

		if (PlaceablesInSight.IsEmpty()) {
			StopShooting();
			StartMoving();
		}
	}
}

void AEnemy::ShootPlaceable(TWeakObjectPtr<ADDPlaceable> Placeable) const
{
	ADDProjectile* Proj = GetWorld()->SpawnActor<ADDProjectile>(Projectile, GetActorLocation() + ProjectileOffset, GetActorRotation());

	if (Proj) {
		Proj->SetProjectileOwner(this);
		Proj->SetCollisionChannelToIgnore(ECC_EnemyChannel);
		Proj->SetCollisionChannelToIgnore(ECC_AttackRadiusChannel);
		Proj->SetCollisionChannelToIgnore(ECC_EnemyRadiusChannel);
		Proj->SetDamage(Damage);

		if (Placeable.IsValid()) {
			FVector PlaceLoc = Placeable->GetActorLocation();
			FVector ThisLoc = GetActorLocation();
			FVector Direction = PlaceLoc - ThisLoc;
			Direction.Normalize();

			Proj->SetVelocity(Direction * ProjectileSpeed);
			Proj->SetDestination(PlaceLoc);
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Placeable fetched is not valid"))
			//Is there a reason for doing this? Projectile already has a default velocity
			FVector Velocity(-ProjectileSpeed, 0, 0);
			Proj->SetVelocity(Velocity);
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("ERROR: Enemy projectile unable to spawn"))
	}
}

void AEnemy::OnDeath()
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->AddScore(Score);
		GameMode->UpdateSouls(SoulValue);
	}

	OnActorDeath.Broadcast(this);
	OnEnemyDeath.ExecuteIfBound(this);
}

void AEnemy::StartShooting()
{	
	//Boolean prevents the Timer Handle from resetting every tick
	if (!bIsShooting) { 
		bIsShooting = true;
		float InitialDelay = bInitialProjectileDelay ? ShootCooldown : 1.0f;
		GetWorldTimerManager().SetTimer(ShootHandle, this, &AEnemy::Shoot, ShootCooldown, true, InitialDelay);
	}
}

void AEnemy::StopShooting()
{
	if (bIsShooting) {
		bIsShooting = false;
		GetWorldTimerManager().ClearTimer(ShootHandle);
	}
}

void AEnemy::StartMoving()
{
	FloatingPawnMovement->MaxSpeed = MovementSpeed;
}

void AEnemy::StopMoving()
{
	FloatingPawnMovement->MaxSpeed = 0;
}

void AEnemy::Shoot()
{	
	OnEnemyAttackCastle.ExecuteIfBound();
	ADDProjectile* Proj = GetWorld()->SpawnActor<ADDProjectile>(Projectile, GetActorLocation() + ProjectileOffset, GetActorRotation());
	
	if (Proj) {
		Proj->SetProjectileOwner(this);
		//Shoot negative x axis just cus castle is that direction
		//Honestly could use vectors to do this dynamically, maybe later
		FVector Velocity(-ProjectileSpeed, 0, 0);
		Proj->SetVelocity(Velocity);
		Proj->SetCollisionChannelToIgnore(ECC_EnemyChannel);
		Proj->SetCollisionChannelToIgnore(ECC_EnemyRadiusChannel);
		Proj->SetCollisionChannelToIgnore(ECC_AttackRadiusChannel);
		Proj->SetDamage(Damage);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("ERROR: Enemy projectile unable to spawn"))
	}
}

void AEnemy::StartShootingPlaceable()
{
	if (!bIsShooting) {
		bIsShooting = true;
		float InitialDelay = bInitialProjectileDelay ? ShootCooldown : 1.0f;
		GetWorldTimerManager().SetTimer(ShootHandle, this, &AEnemy::ShootPlaceable, ShootCooldown, true, InitialDelay);
	}
}

void AEnemy::ShootPlaceable()
{
	if (PlaceablesInSight.Num() <= 0) {
		UE_LOG(LogTemp, Warning, TEXT("There are no placeables in sight! Stopping shooting..."))
		StopShooting();
		return;
	}

	ADDProjectile* Proj = GetWorld()->SpawnActor<ADDProjectile>(Projectile, GetActorLocation() + ProjectileOffset, GetActorRotation());

	if (Proj) {
		Proj->SetProjectileOwner(this);
		Proj->SetCollisionChannelToIgnore(ECC_EnemyChannel);
		Proj->SetCollisionChannelToIgnore(ECC_AttackRadiusChannel);
		Proj->SetCollisionChannelToIgnore(ECC_EnemyRadiusChannel);
		Proj->SetDamage(Damage);

		//Always shooting first Placeable
		ADDPlaceable* Placeable = PlaceablesInSight[0];

		if (Placeable) {
			FVector PlaceLoc = Placeable->GetActorLocation();
			FVector ThisLoc = GetActorLocation();
			FVector Direction = PlaceLoc - ThisLoc;
			Direction.Normalize();

			Proj->SetVelocity(Direction * ProjectileSpeed);
			Proj->SetDestination(PlaceLoc);
			if (Placeable->IsEvil()) {
				PlaceablesInSight.RemoveSwap(Placeable);
				if (PlaceablesInSight.IsEmpty()) {
					StopShooting();
					StartMoving();
				}
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Placeable fetched is not valid"))
				//Is there a reason for doing this? Projectile already has a default velocity
			FVector Velocity(-ProjectileSpeed, 0, 0);
			Proj->SetVelocity(Velocity);
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("ERROR: Enemy projectile unable to spawn"))
	}
}

void AEnemy::ValidateProjectile()
{
	check(Projectile != nullptr);
}

void AEnemy::FindPlayer()
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		// You know, me having this line of code defeats the entire 
		// purpose of GetPlayer returning only a reference. Very awesome
		Player = &(GameMode->GetPlayer());
	}
}

void AEnemy::AdjustAttackBox()
{
	FVector Origin, BoxExtent;
	Mesh->GetLocalBounds(Origin, BoxExtent);
	
	FVector NewBox(AttackSightDistance, BoxExtent.Y, BoxExtent.Z);
	AttackSight->SetBoxExtent(NewBox);

	FVector NewLocation(AttackSightDistance, 0, 0);
	AttackSight->SetRelativeLocation(-NewLocation);
}

void AEnemy::AdjustHealthBarRotation()
{
	if (HealthBarWidget) {
		FRotator HealthRotation = HealthBarWidget->GetComponentRotation();
		FVector HealthLocation = HealthBarWidget->GetComponentLocation();

		APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		FVector CamLocation = CameraManager->GetCameraLocation();

		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(HealthLocation, CamLocation);
		FRotator NewRotation(FRotator(LookAtRotation.Pitch, HealthRotation.Yaw, HealthRotation.Roll));

		HealthBarWidget->SetWorldRotation(NewRotation);
	}
}

void AEnemy::UpdateHealthBar()
{
	if (HealthWidgetRef.IsValid()) {
		HealthWidgetRef->SetPercent(TempHealth / Health);
	}
}
