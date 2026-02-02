// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "DragonsDefense_v2/Characters/LivingActor.h"
#include "Enemy.generated.h"

class UWidgetComponent;
class UFloatingPawnMovement;
class UMaterialBillboardComponent;
class UAudioComponent;
class ADDCastle;
class ADDPlayer;
class ADDProjectile;
class ADDPlaceable;
class UDDEnemyHealthBar;
class UDDTickBasedDebuffBase;
class UDDTimeBasedDebuffBase;
class UDDEnemyDataAsset;

enum class EDDEnemyType : uint8;

DECLARE_DELEGATE_OneParam(FOnEnemyDeath, AEnemy*)
//Used for the occupation squares only to differentiate deathing
DECLARE_MULTICAST_DELEGATE_OneParam(FOnActorDeath, AEnemy*)
DECLARE_DELEGATE(FOnEnemyAttackCastle)

UCLASS()
class DRAGONSDEFENSE_V2_API AEnemy : public ALivingActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this pawn's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	//insert AI component here if needed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Mesh; //TODO - Replace with skeletal mesh when we get to animation
	//Utilized for attacking sentient placeables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* AttackSight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* HealthBarWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UFloatingPawnMovement* FloatingPawnMovement;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UAudioComponent* ShootSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|HealthBar")
	TSubclassOf<UDDEnemyHealthBar> HealthClass;
	//What projectile will the enemy be using?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Projectile");
	TSubclassOf<ADDProjectile> Projectile;
	//Offset of spawned projectile relative to the origin of the enemy
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Projectile");
	FVector ProjectileOffset = FVector::Zero();
	//If there should be an initial delay when the enemy starts shooting
	//true results in a delay of time equal to ShootCooldown
	//false results in a very a small delay
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Projectile");
	bool bInitialProjectileDelay = true;
	//Distance from the castle to attack
	UPROPERTY(EditAnywhere, Category = "Stats|AI");
	float DistanceToAttack = 100.0f;
	//Distance from placeable to attack
	UPROPERTY(EditAnywhere, Category = "Stats|AI");
	float AttackSightDistance = 100.0f;
	UPROPERTY(EditAnywhere, Category = "Stats|AI");
	bool bEnableAttackSentients = false;
	UPROPERTY(EditAnywhere, Category = "Stats|AI");
	bool bEnableAttackTraps = false;
	//Does nothing for changing behavior, only used as an attribute for The King AI to sort aggressive enemies
	UPROPERTY(EditAnywhere, Category = "Stats|AI");
	bool bDestroysPlaceables = false;
	UPROPERTY(EditAnywhere, Category = "Stats|AI")
	bool bOverrideAttackSightColliderSize = false;
	//Leave at zero for no custom spawn range
	UPROPERTY(EditAnywhere, Category = "Stats|AI")
	FVector2D CustomSpawnRange = FVector2D::Zero();
	UPROPERTY(EditAnywhere, Category = "Stats");
	float ShootCooldown = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Stats");
	float MovementSpeed = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Stats");
	float ProjectileSpeed = 800.0f;
	UPROPERTY(EditAnywhere, Category = "Stats");
	int32 SoulValue = 1;
	//How resistant is the enemy to slow debuffs
	//Calculates resistance by taking a percentage away from the actual debuff in DDSlowDebuff.cpp
	//1 - (1 - Debuff) * (1 - Resistance)
	//Close to 0 is not immune to slow debuffs
	//Close to 1 is immune to slow debuffs
	UPROPERTY(EditDefaultsOnly, Category = "Stats", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float SlowDebuffResistance = 0.0f;
	UPROPERTY(EditAnywhere, Category = "Stats");
	int32 Score = 1;
	//Higher rarity means more rare
	UPROPERTY(EditAnywhere, Category = "Stats");
	float Rarity = 1.0f;
	//Should be a unique number for current enemy
	UPROPERTY(EditAnywhere, Category = "Stats");
	int32 EnemyID = 0;
	UPROPERTY(EditAnywhere, Category = "Stats");
	FString EnemyName = "Villager";
	UPROPERTY(EditAnywhere, Category = "Stats");
	FString EnemyDescription = "Description";
	UPROPERTY(EditAnywhere, Category = "Stats");
	FString SpecialAbilityDescription;
	UPROPERTY(EditDefaultsOnly, Category = "Stats");
	EDDEnemyType Type;
	UPROPERTY(BlueprintReadOnly, Category = "Temp")
	float TempHealth;
	UPROPERTY(EditAnywhere, Category = "Stats|Detection")
	bool bIsSentientDetectable = true;
	UPROPERTY(EditAnywhere, Category = "Stats|Detection")
	bool bIsTrapDetectable = true;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Modifiers");
	float EasyMovementSpeedMod = 0.9f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Modifiers");
	float HardMovementSpeedMod = 1.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Modifiers");
	float EasyShootCooldownMod = 1.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Modifiers");
	float HardShootCooldownMod = 0.8f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Modifiers");
	float EasyDamageMod = 0.8f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Modifiers");
	float HardDamageMod = 1.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Modifiers");
	int32 EasySoulValueMod = 1;
	//Currently commented out, not used
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Modifiers");
	int32 HardSoulValueMod = -1;

	void ShootPlaceable(TWeakObjectPtr<ADDPlaceable> Placeable) const;

	virtual void Shoot();
	virtual void ShootPlaceable();
	virtual void OnDeath() override;
	// Checks the distance of self and the castle. 
	// Returns true if in distance to attack
	bool CheckDistance();
	void StartMoving();
	void StopMoving();
	void AdjustHealthBarRotation();
	void StopShooting();

	FTimerHandle ShootHandle;

	TArray<ADDPlaceable*> PlaceablesInSight;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	UDDEnemyDataAsset* EnemyData;

	UStaticMeshComponent* GetMeshComponent() const;
	UFloatingPawnMovement* GetFloatingPawnMovement() const;
	virtual FVector GetColliderSizeWorldSpace() const override;
	//Returns true if the enemy has a custom range, false otherwise
	const bool GetCustomSpawnRange(FVector2D& CustomRange) const;
	const float GetMovementSpeed() const;
	const float GetDistanceFromCastle() const;
	const float GetMaxSpeed() const;
	const float GetRarity() const;
	const float GetProjectileSpeed() const;
	void SetMovementSpeed(const float NewMovementSpeed);
	const bool GetIsSentientDetectable() const;
	const bool GetIsTrapDetectable() const;
	const float GetSlowDebuffResistance() const;
	const int32 GetEnemyID() const;
	const EDDEnemyType GetEnemyType() const;
	const bool GetDestroysPlaceables() const;

	void UpdateHealth(const float HealthModifier) override;

	//Multiplies health by mod
	void SetHealthMod(const float NewHealthMod);
	//Multiplies shoot cooldown by mod
	void SetShootCooldownMod(const float NewShootCooldownMod);
	//Multiplies soul value by mod
	void SetSoulValueMod(const float NewSoulValueMod);
	//Multiplies movement by mod
	void SetMovementMod(const float NewMovementMod);
	//Multiplies damage by mod
	void SetDamageMod(const float NewDamageMod);
	void SetAttackSightVisibility(const bool Visibility);

	//Forces Enemy to attack Placeable passed in
	void ForceAttack(ADDPlaceable* Placeable);

	void ForceRemovePlaceable(ADDPlaceable* Placeable);

	FOnEnemyDeath OnEnemyDeath;
	FOnActorDeath OnActorDeath;
	FOnEnemyAttackCastle OnEnemyAttackCastle;

private:

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void ApplyDifficultyModifiers() override;
	void StartShooting();
	void StartShootingPlaceable();
	// Checks if projectile was assigned to the enemy
	void ValidateProjectile();
	void FindPlayer();
	void AdjustAttackBox();
	void UpdateHealthBar();

	ADDPlayer* Player;
	TWeakObjectPtr<UDDEnemyHealthBar> HealthWidgetRef;

	bool bIsShooting = false;
	float DistanceFromCastle;
};
