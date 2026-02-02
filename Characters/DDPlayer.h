// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/LivingActor.h"
#include "DragonsDefense_v2/Characters/DDPlayerStatInfo.h"
#include "DragonsDefense_v2/Game/DDResettableInterface.h"
#include "DDPlayer.generated.h"

class UCameraComponent;
class UFloatingPawnMovement;
class ADDPlayerProjectile;
class ADDPlaceable;
class ADDGameModeBase;

enum class EDDStressMeterTypes : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnUpdateHealth, float, Health, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnClickPlaceable, FVector2D, CursorScreenPosition, ADDPlaceable*, Placeable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickOutsideOfPlaceable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoverPlaceable, ADDPlaceable*, Placeable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHoverClickable, ADDClickableBase*, Clickable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHoverOutsideClickable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRightClick);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpgradedMaxHealth, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpgradedShootSpeed, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpgradedDamage, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpgradedProjectileSpeed, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpgradedPenetration, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpgradedProjectileSize, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpgradedLineOfSightLength, float);
DECLARE_DELEGATE(FOnUpgradedStat);

UCLASS()
class DRAGONSDEFENSE_V2_API ADDPlayer : public ALivingActor, public IDDResettableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADDPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Mesh; //TODO - Later down the line change to SkeletalMeshComponent
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UFloatingPawnMovement* FloatingPawnMovement;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* LineOfSightMesh;

	UFUNCTION(BlueprintCallable)
	void FindPlaceableWithCursor();
	UFUNCTION(BlueprintCallable)
	void BroadcastRightClick() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Projectile")
	TSubclassOf<ADDPlayerProjectile> Projectile;
	//Where the projectile is spawned relative to the player's center
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Projectile")
	FVector ProjectileOffset = FVector(0, 0, 0); 
	//Base movement speed
	UPROPERTY(EditAnywhere, Category = "Stats")
	float MovementSpeed = 20.0f;
	//Base shooting speed (in seconds)
	UPROPERTY(EditAnywhere, Category = "Stats")
	float ShootSpeed = 0.6f;
	UPROPERTY(EditAnywhere, Category = "Stats")
	float ProjectileSpeed = 75.0f;
	//Amount of LivingActors able to penetrate through
	UPROPERTY(EditAnywhere, Category = "Stats")
	int32 Penetration = 1;
	//Chance to shoot a critical projectile
	UPROPERTY(EditAnywhere, Category = "Stats")
	float CritProjChance = 1.0f;
	//How big the projectile is in all directions (X,Y,Z)
	//Think of it as the radius
	UPROPERTY(EditAnywhere, Category = "Stats")
	float ProjectileSize = 50.0f;
	UPROPERTY(EditAnywhere, Category = "Stats")
	float LineOfSightLength = 150.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	FDDPlayerStatInfo PlayerStatInfo;
	UPROPERTY(BlueprintReadOnly, Category = "Stats|Temp")
	float TempMovementSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "Stats|Temp")
	float TempDamage;
	UPROPERTY(BlueprintReadOnly, Category = "Stats|Temp")
	float TempShootSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "Stats|Temp")
	float TempProjectileSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "Stats|Temp")
	float TempCritProjChance;
	UPROPERTY(BlueprintReadOnly, Category = "Stats|Temp")
	int32 TempPenetration;
	UPROPERTY(BlueprintReadOnly, Category = "Stats|Temp")
	float TempProjectileSize;
	UPROPERTY(BlueprintReadOnly, Category = "Stats|Temp")
	float TempLineOfSightLength;
	//Limits the area of where the player can move on the Y-axis
	UPROPERTY(EditAnywhere, Category = "Stats|Area Limit")
	float AreaLimitY = 50.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool OnCooldown = false;
	//How inaccurate shooting is in the y-axis
	UPROPERTY(BlueprintReadOnly)
	float Inaccuracy = 0;
	UPROPERTY(BlueprintReadWrite)
	//Initialized in blueprints
	UUserWidget* CursorWidget;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ResetMods() override;
	
	//Get max health with no modifiers involved
	const float GetUnModdedMaxHealth() const;
	//Get shoot speed with no modifiers involved
	const float GetUnModdedShootSpeed() const;
	//Get damage with no modifiers involved
	const float GetUnModdedDamage() const;
	//Get projectile speed with no modifiers involved
	const float GetUnModdedProjectileSpeed() const;
	//Get penetration with no modifiers involved
	const float GetUnModdedPenetration() const;
	//Get projectile size with no modifiers involved
	const float GetUnModdedProjectileSize() const;
	//Get line of sight length with no modifiers involved
	const float GetUnModdedLineOfSightLength() const;

	const float GetHealth() const override;
	const float GetDamage() const override;

	UFUNCTION(BlueprintPure, Category = "Getters")
	const float GetMaxHealth() const;
	UFUNCTION(BlueprintPure, Category = "Getters")
	const float GetMovementSpeed() const;
	UFUNCTION(BlueprintPure, Category = "Getters")
	const float GetShootSpeed() const;
	UFUNCTION(BlueprintPure, Category = "Getters")
	const float GetProjectileSpeed() const;
	UFUNCTION(BlueprintPure, Category = "Getters")
	const float GetCritProjChance() const;
	UFUNCTION(BlueprintPure, Category = "Getters")
	const int32 GetPenetration() const;
	UFUNCTION(BlueprintPure, Category = "Getters")
	const float GetProjectileSize() const;
	UFUNCTION(BlueprintPure, Category = "Getters")
	const float GetLineOfSightLength() const;
	UFUNCTION(BlueprintPure, Category = "Getters")
	const bool IsPlacingState() const;
	UFUNCTION(BlueprintPure, Category = "Getters")
	const bool IsWaveOver() const;
	//Wrapper for PlayerStatInfo's function since UFUNCTION can't be applied in structs
	UFUNCTION(BlueprintPure, Category = "Getters")
	const FString GetPlayerStatFuzzification(const EPlayerStats PlayerStat) const;

	void UpdateHealth(const float HealthModifier) override;
	void UpdateMaxHealth(const float MaxHealthModifier);
	void UpdateMovementSpeed(const float MovementSpeedModifier);
	void UpdateShootSpeed(const float ShootSpeedModifier);
	void UpdateDamage(const float DamageModifier);
	void UpdateProjectileSpeed(const float ProjectileSpeedModifier);
	void UpdateCritProjChance(const float CritProjChanceModifier);
	void UpdatePenetration(const int32 PenetrationModifier);
	void UpdateProjectileSize(const float ProjectileSizeModifier);
	void UpdateLineOfSightLength(const float LineOfSightModifier);
	void SetMovementSpeedMod(const float NewMovementSpeedMod);
	void SetDamageMod(const float NewDamageMod);
	void SetShootSpeedMod(const float NewShootSpeedMod);
	void SetMaxHealthMod(const float NewMaxHealthMod);
	//Inaccuracy plus new inaccuracy mod
	void SetInaccuracyMod(const float NewInaccuracyMod);
	void SetProjectileSpeedMod(const float NewProjectileSpeedMod);
	void SetCritProjChanceMod(const float NewCritProjChanceMod);
	void SetPenetrationMod(const float NewPenetrationMod);
	void SetProjectileSizeMod(const float NewProjectileSizeMod);
	void SetLineOfSightLengthMod(const float NewLineOfSightLengthMod);

	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnUpdateHealth OnUpdateHealth;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnClickPlaceable OnClickPlaceable;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnClickOutsideOfPlaceable OnClickOutsideOfPlaceable;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnHoverPlaceable OnHoverPlaceable;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnHoverClickable OnHoverClickable;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnHoverOutsideClickable OnHoverOutsideClickable;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnRightClick OnRightClick;

	FOnUpgradedProjectileSpeed OnUpgradedProjectileSpeed;
	FOnUpgradedMaxHealth OnUpgradedMaxHealth;
	FOnUpgradedShootSpeed OnUpgradedShootSpeed;
	FOnUpgradedDamage OnUpgradedDamage;
	FOnUpgradedPenetration OnUpgradedPenetration;
	FOnUpgradedProjectileSize OnUpgradedProjectileSize;
	FOnUpgradedLineOfSightLength OnUpgradedLineOfSightLength;
	FOnUpgradedStat OnUpgradedStat;

private:
	/// <summary>
	/// Returns the result for if the mouse is currently hovering over a clickable
	/// </summary>
	/// <param name="Hit">Information of the Raycast hit, by reference</param>
	/// <returns>True if hovering over a clickable, false otherwise</returns>
	bool MouseOnClickable(FHitResult &Hit);
	void ValidateProjectile();
	//Updates the length of the line of sight mesh appropriately
	void UpdateLineOfSight();
	void ResetStats();
	/**
	 * @brief Limits area the player can move in
	 * 
	 */
	void LimitArea();
	//Check if mouse is hovering over something that is clickable
	void CheckMousePosition();
	void OnDeath() override;

	UFUNCTION()
	void GameOverEventFunction();
	UFUNCTION()
	void GameStartEventFunction();
	UFUNCTION()
	void GameRestartEventFunction();
	UFUNCTION()
	void WaveOverEventFunction();
	UFUNCTION()
	void WaveStartEventFunction();
	UFUNCTION()
	void PlacementEventFunction(bool IsPlacing);

	bool bIsPlacing = false;
	bool bIsWaveOver = false;
	bool bIsHoveringPlaceableOrClickable = false;
	
	float MaxHealth;
	float TempHealth;
	FVector OriginalLocation;

	float ActualMovementSpeed;
	float ActualDamage;
	float ActualShootSpeed;
	float ActualMaxHealth;
	float ActualInaccuracy;
	float ActualProjectileSpeed;
	float ActualCritProjChance;
	int32 ActualPenetration;
	float ActualProjectileSize;
	float ActualLineOfSightLength;

	float MovementSpeedMod = 1;
	float DamageMod = 1;
	float ShootSpeedMod = 1;
	float MaxHealthMod = 1;
	float ProjectileSpeedMod = 1;
	float CritProjChanceMod = 1;
	int32 PenetrationMod = 1;
	float ProjectileSizeMod = 1;
	float LineOfSightLengthMod = 1;

	ADDGameModeBase* GameModeRef;

	//Weak pointer to current actor being hovered over
	TWeakObjectPtr<AActor> CurrentHoverActor;

	APlayerController* PController;
};
