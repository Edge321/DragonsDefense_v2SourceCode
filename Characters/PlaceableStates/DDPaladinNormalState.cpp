// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPaladinNormalState.h"
//My classes
#include "DragonsDefense_v2/Characters/Enemy.h"
#include "DragonsDefense_v2/Characters/DDPlaceable.h"
#include "DragonsDefense_v2/Characters/DDPlaceableAI.h"

void UDDPaladinNormalState::OnInitialize(AActor* Owner)
{
	//Really make sure nothing is in there
	EnemiesInArea.Empty();
	EvilsInArea.Empty();
	if (Owner) {
		SetOwner(Owner);
	}
	else {
		PrintNullPtrError();
	}
	SentientOwner->SetAttackFunction(std::bind(&UDDPaladinNormalState::BeginAttack, this));
	SentientOwner->OnTurnNormal.ExecuteIfBound(SentientOwner.Get());

	StoreAnyEnemyOrEvil(SentientOwner->GetAllActorsInRadius());
	SentientOwner->GetWorldTimerManager().SetTimer(CheckEvilsTimer, this, &UDDPaladinNormalState::CheckForEvils, CheckEvilInterval, true);
}

void UDDPaladinNormalState::HandleBeginOverlap(AActor* Actor)
{
	if (Actor) {
		if (Actor->IsA<AEnemy>()) {
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			if (Enemy->GetIsSentientDetectable()) {
				EnemiesInArea.Add(MakeWeakObjectPtr(Enemy));
				SentientOwner->StartAttack();
			}
		}
		else if (Actor->IsA<ADDPlaceable>()) {
			ADDPlaceable* Placeable = Cast<ADDPlaceable>(Actor);
			if (Placeable->IsEvil()) {
				EvilsInArea.Add(MakeWeakObjectPtr(Placeable));
				SentientOwner->StartAttack();
			}
		}
	}
}

void UDDPaladinNormalState::HandleEndOverlap(AActor* Actor)
{
	if (Actor) {
		if (Actor->IsA<AEnemy>()) {
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			EnemiesInArea.RemoveSwap(MakeWeakObjectPtr(Enemy));
		}
		else if (Actor->IsA<ADDPlaceable>()) {
			ADDPlaceable* Placeable = Cast<ADDPlaceable>(Actor);
			EvilsInArea.RemoveSwap(MakeWeakObjectPtr(Placeable));
		}

		if (EnemiesInArea.IsEmpty() && EvilsInArea.IsEmpty()) {
			SentientOwner->Deactive();
		}
	}
}

void UDDPaladinNormalState::OnUninitialize()
{
	EnemiesInArea.Empty();
	EvilsInArea.Empty();
	if (SentientOwner.IsValid()) {
		SentientOwner->Deactive();
		SentientOwner->GetWorldTimerManager().ClearTimer(CheckEvilsTimer);
	}
	else {
		CheckEvilsTimer.Invalidate();
	}
	SentientOwner.Reset();
}

void UDDPaladinNormalState::BeginDestroy()
{
	OnUninitialize();

	Super::BeginDestroy();
}

void UDDPaladinNormalState::StoreAnyEnemyOrEvil(TArray<AActor*> Actors)
{
	for (AActor* Actor : Actors) {
		if (Actor && Actor->IsA<AEnemy>()) {
			if (Actor->IsA<AEnemy>()) {
				AEnemy* Enemy = Cast<AEnemy>(Actor);
				EnemiesInArea.Add(MakeWeakObjectPtr(Enemy));
			}
			else if (Actor->IsA<ADDPlaceable>()) {
				ADDPlaceable* Placeable = Cast<ADDPlaceable>(Actor);
				if (Placeable->IsEvil()) {
					EvilsInArea.Add(MakeWeakObjectPtr(Placeable));
				}
			}
		}
	}

	if (!EnemiesInArea.IsEmpty() || !EvilsInArea.IsEmpty()) {
		SentientOwner->StartAttack();
	}
}

void UDDPaladinNormalState::BeginAttack()
{
	if (EnemiesInArea.IsEmpty() && EvilsInArea.IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("Enemies or evils not in area but %s is still attacking"), *GetName())
		SentientOwner->Deactive();
		return;
	}

	if (!EvilsInArea.IsEmpty()) {
		SearchForEvil();
	}
	else {
		SearchForEnemy();
	}
}

void UDDPaladinNormalState::SearchForEvil()
{
	if (CurrentlyTargetedEvil.IsValid()) {
		SentientOwner->Attack(CurrentlyTargetedEvil.Get(), true, SmiteEvilDamage);
	}
	else {
		int RandomIndex = FMath::Rand() % EvilsInArea.Num();
		CurrentlyTargetedEvil = EvilsInArea[RandomIndex];
		SentientOwner->Attack(CurrentlyTargetedEvil.Get(), true, SmiteEvilDamage);
	}
}

void UDDPaladinNormalState::CheckForEvils()
{
	if (SentientOwner.IsValid()) {
		TArray<AActor*> Actors = SentientOwner->GetAllActorsInRadius();
		for (AActor* Actor : Actors) {
			if (Actor && Actor->IsA<ADDPlaceable>()) {
				ADDPlaceable* Placeable = Cast<ADDPlaceable>(Actor);
				if (Placeable->IsEvil()) {
					EvilsInArea.Add(MakeWeakObjectPtr(Placeable));
					SentientOwner->StartAttack();
				}
			}
		}
	}
	else {
		CheckEvilsTimer.Invalidate();
	}
}

void UDDPaladinNormalState::SearchForEnemy()
{
	int RandomIndex;
	EPlaceableAI CurrentAI = SentientOwner->GetAI();

	switch (CurrentAI) {
	case EPlaceableAI::Deactivated:
		UE_LOG(LogTemp, Warning, TEXT("%s is deactivated but in attack state"), *GetName())
			break;
	case EPlaceableAI::ClosestEnemy:
		SentientOwner->Attack(FindMinMaxEnemy(false, [&](TWeakObjectPtr<AEnemy> Enemy) -> float {
			return FVector::Distance(Enemy->GetActorLocation(), SentientOwner->GetActorLocation());
			}));
		break;
	case EPlaceableAI::FurthestEnemy:
		SentientOwner->Attack(FindMinMaxEnemy(true, [&](TWeakObjectPtr<AEnemy> Enemy) -> float {
			return FVector::Distance(Enemy->GetActorLocation(), SentientOwner->GetActorLocation());
			}));
		break;
	case EPlaceableAI::ClosestToCastle:
		SentientOwner->Attack(FindMinMaxEnemy(false, [](TWeakObjectPtr<AEnemy> Enemy) -> float {
			return Enemy->GetDistanceFromCastle();
			}));
		break;
	case EPlaceableAI::FurthestFromCastle:
		SentientOwner->Attack(FindMinMaxEnemy(true, [](TWeakObjectPtr<AEnemy> Enemy) -> float {
			return Enemy->GetDistanceFromCastle();
			}));
		break;
	case EPlaceableAI::RoundRobin:
		//Make sure its still within bounds
		RobinIndex %= EnemiesInArea.Num();
		SentientOwner->Attack(EnemiesInArea[RobinIndex].Get());
		RobinIndex = (RobinIndex + 1) % EnemiesInArea.Num();
		break;
	case EPlaceableAI::Random:
		RandomIndex = FMath::Rand() % EnemiesInArea.Num();
		SentientOwner->Attack(EnemiesInArea[RandomIndex].Get());
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("%s does not have a valid AI set"), *GetName())
	}
}

AEnemy* UDDPaladinNormalState::FindMinMaxEnemy(const bool IsMax, std::function<float(TWeakObjectPtr<AEnemy>)> ValueFunc)
{
	if (EnemiesInArea.Num() <= 0) {
		UE_LOG(LogTemp, Error, TEXT("Error: EnemiesInArea array is empty"))
			return nullptr;
	}

	TWeakObjectPtr<AEnemy> ChosenEnemy = EnemiesInArea[0];

	if (!ChosenEnemy.IsValid()) {
		PrintNullPtrError();
		return nullptr;
	}

	float CurrentValue = ValueFunc(ChosenEnemy);

	for (TWeakObjectPtr<AEnemy> SomeEnemy : EnemiesInArea) {
		if (SomeEnemy.IsValid()) {
			float TmpDist = ValueFunc(SomeEnemy);
			if (IsMax ? (TmpDist > CurrentValue) : (TmpDist < CurrentValue)) {
				ChosenEnemy = SomeEnemy;
				CurrentValue = TmpDist;
			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Invalid pointer found in EnemiesInArea area"))
		}
	}

	return ChosenEnemy.Get();
}
