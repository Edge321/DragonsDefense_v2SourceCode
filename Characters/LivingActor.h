// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "DragonsDefense_v2/Characters/DDSecretStats.h"
#include "DragonsDefense_v2/Characters/DDLivingActorStyle.h"
#include "LivingActor.generated.h"

class UDDDebuffBase;
class UDDTickBasedDebuffBase;
class UDDTimeBasedDebuffBase;

/**
 * An abstract class meant for actors that have health
 */
UCLASS(Abstract)
class DRAGONSDEFENSE_V2_API ALivingActor : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ALivingActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* Collider;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* Arrow;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float Damage = -1.0f;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Stats");
	float Health = 1.0f;
	//Modifies health if Easy mode was chosen
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Modifiers");
	float EasyHealthMod = 0.8f;
	//Modifies health if Hard mode was chosen
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Modifiers");
	float HardHealthMod = 1.2f;
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Secret Stats");
	FDDSecretStats SecretStats;

	virtual void ApplyDifficultyModifiers() PURE_VIRTUAL(ALivingActor::ApplyDifficultyModifiers, );
	virtual void OnDeath() PURE_VIRTUAL(ALivingActor::OnDeath, );

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure)
	virtual const float GetHealth() const;
	UFUNCTION(BlueprintPure)
	virtual const float GetDamage() const;
	UFUNCTION(BlueprintPure)
	const EDDLivingActorStyle GetLivingActorStyle() const;
	UFUNCTION(BlueprintPure)
	const FDDSecretStats GetSecretStats() const;
	//Returns the size of the collider by taking into account any scales that affect the collider's size
	virtual FVector GetColliderSizeWorldSpace() const;

	void SetDamage(const float NewDamage);

	virtual void UpdateHealth(const float HealthModifier);
	//Equivalent to SetDamage(GetDamage() + float)
	void UpdateDamage(const float NewDamage);

	void UpdateSecretStats(const FDDSecretStats NewSecretStats);

	void AddDebuff(UDDDebuffBase* Debuff);
	void AddDebuff(UDDTickBasedDebuffBase* Debuff);
	void AddDebuff(UDDTimeBasedDebuffBase* Debuff);
	void RemoveDebuff(UDDDebuffBase* Debuff);
	void UpdateDebuffs(float DeltaTime);

private:

	UPROPERTY()
	TArray<UDDDebuffBase*> ActiveDebuffs;
	UPROPERTY()
	TArray<TWeakObjectPtr<UDDTickBasedDebuffBase>> TickDebuffRefs;

	EDDLivingActorStyle LivingActorStyle;

	//Used for if the living actor has had their secret stats updated
	//Meant to be updated only once
	bool bHasUpdatedSecretStats = false;

};
