// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDDebuffAreaTrap.h"
//My classes
#include "DragonsDefense_v2/Characters/Enemy.h"
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/Managers/DDDebuffAreaManager.h"
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"

void ADDDebuffAreaTrap::BeginPlay()
{
	Super::BeginPlay();

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		ADDDebuffAreaManager& DebuffManager = GameMode->GetDebuffAreaManager();
		if (IsValid(&DebuffManager)) {
			ADDDebuffAreaBase* Debuff = DebuffManager.SpawnDebuffArea(DebuffAreaClass, 
																	GetActorLocation(), 
																	FRotator::ZeroRotator, 
																	true);
			if (Debuff) {
				DebuffAreaRef = MakeWeakObjectPtr(Debuff);
				DebuffAreaRef->SetAreaOwner(this);
				DebuffAreaRef->SetDebuffAmount(Damage);
				DebuffAreaRef->SetCollisionChannelToIgnore(ECC_PlaceableChannel);
			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("%s unable to fetch Debuff Manager, unable to use DebuffArea"), *GetName())
		}
	}
}

void ADDDebuffAreaTrap::AdjustDebuffArea()
{
	//Adjust size here
}

void ADDDebuffAreaTrap::OnDeath()
{
	OnPlaceableDeath.Broadcast(this);
	DebuffAreaRef->DebuffAreaDeath();
}

void ADDDebuffAreaTrap::HandleBeginOverlap(AActor* Actor)
{
	if (Actor->IsA<AEnemy>()) {
		AEnemy* Enemy = Cast<AEnemy>(Actor);
		EnemiesInArea.Add(Enemy);
		EnableDebuffArea();
	}
}

void ADDDebuffAreaTrap::HandleEndOverlap(AActor* Actor)
{
	if (Actor->IsA<AEnemy>()) {
		AEnemy* Enemy = Cast<AEnemy>(Actor);
		EnemiesInArea.RemoveSwap(Enemy);
		if (EnemiesInArea.IsEmpty()) {
			DisableDebuffArea();
		}
	}
}

void ADDDebuffAreaTrap::ConvertToEvil()
{
	DebuffAreaRef->DisableDebuffArea();

	Super::ConvertToEvil();
}
