// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDProjectile.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;
class UAudioComponent;
class UNiagaraComponent;
class ALivingActor;

DECLARE_DELEGATE_OneParam(FOnProjectileDestroyed, ADDProjectile*)

/**
 * The Projectile class is used to damage any LivingActor in the world
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADDProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* Collider;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* ImpactSound;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* ProjectileEffects;

	//Set to true if you want to strictly use ProjectileLifetime
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	bool bOverrideProjectileTime = false;
	//Length of life in seconds
	UPROPERTY(EditDefaultsOnly, Category = "Stats");
	float ProjectileLifetime = 10.0f;

	virtual void DestroySelf();
	virtual void HandleHit(ALivingActor* Living);

	float ProjectileDamage = -1.0f;
	TArray<ECollisionChannel> ChannelsIgnored;
	const AActor* ActorOwner;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Getters")
	const float GetDamage() const;
	UFUNCTION(BlueprintPure, Category = "Getters")
	const FVector GetProjectileVelocity() const;
	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetDamage(const float Damage);
	// Who owns the projectile
	UFUNCTION(BlueprintCallable, Category = "Setter")
	void SetProjectileOwner(const AActor* Actor);
	UFUNCTION(BlueprintCallable, Category = "Setter")
	const AActor* GetProjectileOwner() const;
	UFUNCTION(BlueprintCallable, Category = "Setter")
	void SetCollisionChannelToIgnore(const ECollisionChannel Channel);
	UFUNCTION(BlueprintCallable, Category = "Setter")
	void SetVelocity(const FVector Velocity);
	//Should be called AFTER every other modification function used.
	//Reason is to calculate the correct timing for projectile to be destroyed
	//TimeTolerance is how much lifetime (in seconds) allowed after meeting Destination
	UFUNCTION(BlueprintCallable, Category = "Setter")
	void SetDestination(const FVector NewDestination, const float TimeTolerance = 0.0f);
	//Used for if the projectile should collide with only a specific LivingActor, and nothing else
	void SetSpecificTarget(ALivingActor* Living);
	const UStaticMesh* GetStaticMesh() const;

	//Destroys the projectile while cleaning up references to it
	void ForceDestroyProjectile();

	FOnProjectileDestroyed OnProjectileDestroyed;

private:
	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
	
	void EnableCollision();
	//Checks if there is a possible collsion between now and last frame
	void CheckPossibleCollisionPath();

	bool bDestinationSetFlag = false;
	FTimerHandle ProjectileTimer;
	FTimerHandle DestinationTimer;
	//ID of projectile's owner
	uint32 OwnerID;

	FVector LastPosition;

	TWeakObjectPtr<ALivingActor> SpecificTarget;

	const float ZLocation = 100.0f;
};
