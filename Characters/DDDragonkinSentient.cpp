// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDDragonkinSentient.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Characters/DDPlayer.h"
#include "DragonsDefense_v2/Projectile/DDPlayerProjectile.h"

void ADDDragonkinSentient::BeginPlay()
{
	Super::BeginPlay();

	InheritPlayerStats();
}

void ADDDragonkinSentient::InitializeProjectile(ADDProjectile* Proj, const float CustomDamage) const
{
	Super::InitializeProjectile(Proj, CustomDamage);

	if (Proj->IsA<ADDPlayerProjectile>()) {
		ADDPlayerProjectile* PlayProj = Cast<ADDPlayerProjectile>(Proj);
		PlayProj->SetProjectileSize(ProjectileSize);
		PlayProj->SetMaxObjectsToPenetrate(Penetration);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Warning: Dragonkin does not have the correct projectile. Some stats will not be inherited"))
	}
}

void ADDDragonkinSentient::InheritPlayerStats()
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		ADDPlayer& Player = GameMode->GetPlayer(); //BUG - If Dragonkin spawned at beginning by PlaceableManager, Player will be null
		if (IsValid(&Player)) {
			SetHealth(Player.GetUnModdedMaxHealth());
			SetAttackSpeed(Player.GetUnModdedShootSpeed());
			SetDamage(Player.GetUnModdedDamage());
			SetProjectileSize(Player.GetUnModdedProjectileSize());
			SetPenetration(Player.GetUnModdedPenetration());
			Player.OnUpgradedMaxHealth.AddUObject(this, &ADDDragonkinSentient::SetHealth);
			Player.OnUpgradedShootSpeed.AddUObject(this, &ADDDragonkinSentient::SetAttackSpeed);
			Player.OnUpgradedDamage.AddUObject(this, &ADDDragonkinSentient::SetDamage);
			Player.OnUpgradedPenetration.AddUObject(this, &ADDDragonkinSentient::SetPenetration);
			Player.OnUpgradedProjectileSize.AddUObject(this, &ADDDragonkinSentient::SetProjectileSize);
			Player.OnUpgradedProjectileSpeed.AddUObject(this, &ADDDragonkinSentient::SetProjectileSpeed);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("%s unable to fetch Player, unable to use inherit from Player"), *GetName())
		}
	}
}

void ADDDragonkinSentient::SetHealth(const float NewHealth)
{
	TempHealth = NewHealth;
}

void ADDDragonkinSentient::SetAttackSpeed(const float NewAttackSpeed)
{
	ActualAttackSpeed = NewAttackSpeed;
	//Should be fine updating AttackSpeed here, as PlaceableManager applys the mods after BeginPlay()
	AttackSpeed = ActualAttackSpeed;
}

void ADDDragonkinSentient::SetProjectileSpeed(const float NewProjectileSpeed)
{
	ProjectileSpeed = NewProjectileSpeed;
}

void ADDDragonkinSentient::SetDamage(const float NewDamage)
{
	ActualDamage = NewDamage;
	Damage = ActualDamage;
}

void ADDDragonkinSentient::SetPenetration(const int32 NewPenetration)
{
	Penetration = NewPenetration;
}

void ADDDragonkinSentient::SetProjectileSize(const float NewProjectileSizeMod)
{
	ProjectileSize = NewProjectileSizeMod;
}
