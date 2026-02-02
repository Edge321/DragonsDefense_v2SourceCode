// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DragonsDefense_v2/Characters/LivingActor.h"
#include "DragonsDefense_v2/Game/Debuffs/DDDebuffBase.h"
#include "DDDebuffAreaInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDDDebuffAreaInterface : public UInterface
{
	GENERATED_BODY()
};

struct FDebuffPair {

public:

	TWeakObjectPtr<ALivingActor> Living;
	TWeakObjectPtr<UDDDebuffBase> Debuff;

	bool operator==(const FDebuffPair& Other) const {
		return Living == Other.Living && Debuff == Other.Debuff;
	}

};

/**
 * 
 */
class DRAGONSDEFENSE_V2_API IDDDebuffAreaInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.


protected:

	TArray<FDebuffPair> PairArray;

	TWeakObjectPtr<AActor> AreaOwner;

	UFUNCTION()
	virtual void OverlapAreaBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
		bool bFromSweep, const FHitResult& SweepResult) PURE_VIRTUAL(IDDDebuffAreaInterface::OverlapAreaBegin, );
	UFUNCTION()
	virtual void OverlapAreaEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) PURE_VIRTUAL(IDDDebuffAreaInterface::OverlapAreaEnd,);

	//Adds pair to array and adds debuff to passed in LivingActor
	virtual void AddDebuffPair(ALivingActor* Living) PURE_VIRTUAL(IDDDebuffAreaInterface::AddDebuffPair, );

	//Removes pair from array based on LivingActor and removes debuff from it
	void RemoveDebuffPair(ALivingActor* Living);
	//Removes pair from array based on Debuff and removes debuff from attached LivingActor
	void RemoveDebuffPair(UDDDebuffBase* Debuff);

public:

	virtual void SetCollisionChannelToIgnore(ECollisionChannel Channel) PURE_VIRTUAL(IDDDebuffAreaInterface::SetCollisionChannelToIgnore, );

	void RemoveDebuffs();

};
