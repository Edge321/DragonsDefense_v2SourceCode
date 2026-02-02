// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDDebuffAreaProjectileBase.h"
//My classes
#include "DragonsDefense_v2/Game/Debuffs/DDTickBasedDebuffBase.h"

ADDDebuffAreaProjectileBase::ADDDebuffAreaProjectileBase()
{
	Collider->OnComponentBeginOverlap.AddDynamic(this, &ADDDebuffAreaProjectileBase::OverlapAreaBegin);
	Collider->OnComponentEndOverlap.AddDynamic(this, &ADDDebuffAreaProjectileBase::OverlapAreaEnd);
}

void ADDDebuffAreaProjectileBase::OverlapAreaBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//This function is not necessary
	if (OtherActor && OtherActor->IsA<ALivingActor>()) {
		ALivingActor* Living = Cast<ALivingActor>(OtherActor);
		//HandleHit(Living);
	}
}

void ADDDebuffAreaProjectileBase::OverlapAreaEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA<ALivingActor>()) {
		ALivingActor* Living = Cast<ALivingActor>(OtherActor);
		RemoveDebuffPair(Living);
	}
}

void ADDDebuffAreaProjectileBase::HandleHit(ALivingActor* Living)
{
	//Potential performance issue, as this is checked every Tick due to CheckPossibleCollisionPath
	bool bHasDebuffAlready = false;
	for (FDebuffPair Pair : PairArray) {
		if (Pair.Living == Living) {
			bHasDebuffAlready = true;
			break;
		}
	}

	//Avoid duplications on same LivingActor
	if (!bHasDebuffAlready) {
		AddDebuffPair(Living);
	}
}

void ADDDebuffAreaProjectileBase::AddDebuffPair(ALivingActor* Living)
{
	FDebuffPair DebuffPair;

	if (Living) {
		if (DebuffClass->IsChildOf<UDDTickBasedDebuffBase>()) {
			UDDTickBasedDebuffBase* Debuff = NewObject<UDDTickBasedDebuffBase>(this, DebuffClass);
			Debuff->SetDebuffAmount(ProjectileDamage);
			Debuff->SetTickInterval(DebuffTickInterval);
			Living->AddDebuff(Debuff);

			DebuffPair.Debuff = MakeWeakObjectPtr(Debuff);
			DebuffPair.Living = MakeWeakObjectPtr(Living);
			PairArray.Add(DebuffPair);
		}
		else {
			UE_LOG(LogTemp, Fatal, TEXT("Incorrect class used. Should be class of UDDTickBasedDebuffBase"))
		}
	}
}

void ADDDebuffAreaProjectileBase::DestroySelf()
{
	RemoveDebuffs();
	OnProjectileDestroyed.ExecuteIfBound(this);
}

void ADDDebuffAreaProjectileBase::SetCollisionChannelToIgnore(ECollisionChannel Channel)
{
	Collider->SetCollisionResponseToChannel(Channel, ECR_Ignore);
}
