// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/DDPlaceable.h"
#include "DDTrapPlaceable.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDTrapPlaceable : public ADDPlaceable
{
	GENERATED_BODY()
	
public:

	ADDTrapPlaceable();


protected:

	virtual void BeginPlay() override;

	virtual void HandleBeginOverlap(AActor* Actor);
	virtual void HandleEndOverlap(AActor* Actor);
	const UStaticMeshComponent* GetMesh() const override;
	void EnableHighlight() override;
	void DisableHighlight() override;
	void OnSpawnOverlap(TArray<AActor*> OverlapActors) override;
	void Explode();
	void OnExplode();
	void AddChannelHurtByExplosion(ECollisionChannel Channel);
	void RemoveChannelHurtByExplosion(ECollisionChannel Channel);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Projectiles")
	bool bIgnoreProjectiles = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Explosion")
	float ExplosionRadius = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Explosion")
	float ExplosionDamage = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats|Explosion")
	float SecondsTilExplode = 3.0f;

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:

	void ConvertToEvil() override;
	
private:

	FTimerHandle ExplodeTimer;

	TArray<ECollisionChannel> ChannelsForExplosion;

};
