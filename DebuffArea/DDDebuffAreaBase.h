// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DragonsDefense_v2/DebuffArea/DDDebuffAreaInterface.h"
#include "DDDebuffAreaBase.generated.h"

class UBoxComponent;
class UDDDebuffBase;
class ALivingActor;

DECLARE_DELEGATE_OneParam(FOnDeath, ADDDebuffAreaBase*);

UCLASS()
class DRAGONSDEFENSE_V2_API ADDDebuffAreaBase : public AActor, public IDDDebuffAreaInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADDDebuffAreaBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* Collider;

	UPROPERTY(EditDefaultsOnly, Category = "Stats|Debuff")
	TSubclassOf<UDDDebuffBase> DebuffClass;
	//If the DebuffArea lives forever
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	bool bIsImmortal = false;
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	bool bIsEnabledAtBeginning = true;
	//If the debuff area should have the ability to 
	//remove a debuff from an enemy after overlap end
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	bool bCanRemoveDebuff = true;
	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float LifeTime = 10.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Debuff")
	float DebuffAmount = 1;
	
	virtual void AddDebuffPair(ALivingActor* Living) override;

public:
	
	const FVector GetActualMeshSize() const;
	void SetAreaOwner(AActor* NewAreaOwner);
	void SetCollisionChannelToIgnore(const ECollisionChannel Channel) override;
	void SetDebuffAmount(const float NewDebuffAmount);
	void EnableDebuffArea();
	void DisableDebuffArea();
	void DebuffAreaDeath();

	FOnDeath OnDeath;

private:

	void OverlapAreaBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	void OverlapAreaEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	void CheckDebuffClass() const;
	
	void EnableCollision();

	FTimerHandle LifetimeTimer;
	FTimerHandle NoCollisionTimer;
};
