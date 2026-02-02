// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/DDPlaceable.h"
#include <functional>
#include "DDSentientPlaceable.generated.h"

class USphereComponent;
class AEnemy;
class ADDProjectile;
class ADDMageSniperEnemy;

enum class EPlaceableAI : uint8;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDSentientPlaceable : public ADDPlaceable
{
	GENERATED_BODY()
	
public:

	ADDSentientPlaceable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	//TODO - Wayyy later down the line, replace this with a skeletal mesh for animations
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* AttackCollider;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* RadiusMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats|Projectile");
	TSubclassOf<ADDProjectile> Projectile;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float AttackRadius = 25.0f;
	//How fast Placeable attacks in seconds
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float AttackSpeed = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Projectile")
	float ProjectileSpeed = 500.0f;
	//How much (in seconds) can the projectile last after reaching its calculated destination
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Projectile")
	float ProjectileTimeTolerance = 0.05f;
	//0 for perfect accuracy, >0 for less accuracy
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float Inaccuracy = 0;

	float ActualAttackRadius;
	float ActualAttackSpeed;
	float ActualInaccuracy;

	virtual void InitializeProjectile(ADDProjectile* Proj, const float CustomDamage) const;

public:

	void OnConstruction(const FTransform& Transform) override;
	
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetAI(const EPlaceableAI AIState);
	UFUNCTION(BlueprintPure, Category = "AI")
	EPlaceableAI GetAI() const;
	UFUNCTION(BlueprintCallable, Category = "AttackRadius")
	void ShowAttackRadius();
	UFUNCTION(BlueprintCallable, Category = "AttackRadius")
	void HideAttackRadius();
	
	void EnableHighlight() override;
	void DisableHighlight() override;
	const UStaticMeshComponent* GetMesh() const;
	const float GetAttackRadius() const;
	const FVector GetRadiusMeshSize() const;
	TArray<AActor*> GetAllActorsInRadius() const;

	virtual void ConvertToEvil() override;
	void StartAttack();
	void Deactive();
	void EnableAI();
	void DisableAI();
	//Takes movement speed into account
	void Attack(AEnemy* Enemy) const;
	//Assumes actor is stationary. Enable specific target to only damage passed in Actor
	void Attack(AActor* Enemy, const bool bIsSpecificTarget = false, const float CustomDamage = 1.0f) const;

	//Current attack radius + new attack radius
	void UpdateAttackRadius(const float NewAttackRadius);
	void SetAttackFunction(std::function<void()> Func);
	//Original attack radius multiplied by mod
	void SetAttackRadiusMod(const float AttackRadiusMod);
	//Original attack speed multiplied by mod
	void SetAttackSpeedMod(const float AttackSpeedMod);
	//Original accuracy plus mod
	void SetAccuracyMod(const float AccuracyMod);

private:

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//If a sniper in attack radius suddenly turns detectable
	void SniperTurnDetectable(ADDMageSniperEnemy* Sniper);
	void InitializeState();
	void ValidateProjectile();
	//Wrapper function for work-around with FTimerHandle
	void TriggerAttack();
	void StopAttack();
	void AdjustAttackMesh();

	std::function<void()> AttackFunc;

	float TempPrice;

	bool bIsAttacking = false;
	bool bIsAIActive = true;
	int32 RobinIndex = 0;

	FTimerHandle CooldownHandle;
	FTimerHandle AttackHandle;
	EPlaceableAI CurrentAI;
	TArray<ADDPlaceable*> PlaceablesInArea;
	
};
