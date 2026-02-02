// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDNormalSentientState.h"
//My classes
#include <functional>
#include "DragonsDefense_v2/Characters/Enemy.h"
#include "DragonsDefense_v2/Characters/DDPlaceableAI.h"

void UDDNormalSentientState::OnInitialize(AActor* Owner)
{
	//Make sure nothing is in here
	EnemiesInArea.Empty();
	if (Owner) {
		SetOwner(Owner);
	}
	else {
		PrintNullPtrError();
	}
	SentientOwner->SetAttackFunction(std::bind(&UDDNormalSentientState::BeginAttack, this));

	StoreAnyEnemy(SentientOwner->GetAllActorsInRadius());
}

void UDDNormalSentientState::HandleBeginOverlap(AActor* Actor)
{
	if (Actor && Actor->IsA<AEnemy>()) {
		AEnemy* Enemy = Cast<AEnemy>(Actor);
		if (Enemy->GetIsSentientDetectable()) {
			EnemiesInArea.Add(Enemy);
			SentientOwner->StartAttack();
		}
	}
}

void UDDNormalSentientState::HandleEndOverlap(AActor* Actor)
{
	if (Actor && Actor->IsA<AEnemy>()) {
		AEnemy* Enemy = Cast<AEnemy>(Actor);
		EnemiesInArea.RemoveSwap(Enemy);
		if (EnemiesInArea.IsEmpty()) {
			SentientOwner->Deactive();
		}
	}
}

void UDDNormalSentientState::OnUninitialize()
{
	EnemiesInArea.Empty();
	if (SentientOwner.IsValid()) {
		SentientOwner->Deactive();
	}
	SentientOwner.Reset();
}

void UDDNormalSentientState::StoreAnyEnemy(TArray<AActor*> Actors)
{
	for (AActor* Actor : Actors) {
		if (Actor && Actor->IsA<AEnemy>()) {
			AEnemy* Enemy = Cast<AEnemy>(Actor);
			EnemiesInArea.Add(Enemy);
		}
	}
}

void UDDNormalSentientState::BeginAttack()
{
	if (EnemiesInArea.Num() <= 0) {
		UE_LOG(LogTemp, Warning, TEXT("Enemies not in area but %s is still attacking"), *GetName())
			return;
	}

	EPlaceableAI CurrentAI = SentientOwner->GetAI();

	int RandomIndex;

	switch (CurrentAI) {
	case EPlaceableAI::Deactivated:
		UE_LOG(LogTemp, Warning, TEXT("%s is deactivated but in attack state"), *GetName())
			break;
	case EPlaceableAI::ClosestEnemy:
		SentientOwner->Attack(FindMinMaxEnemy(false, [&](AEnemy* Enemy) -> float {
			return FVector::Distance(Enemy->GetActorLocation(), SentientOwner->GetActorLocation());
			}));
		break;
	case EPlaceableAI::FurthestEnemy:
		SentientOwner->Attack(FindMinMaxEnemy(true, [&](AEnemy* Enemy) -> float {
			return FVector::Distance(Enemy->GetActorLocation(), SentientOwner->GetActorLocation());
			}));
		break;
	case EPlaceableAI::ClosestToCastle:
		SentientOwner->Attack(FindMinMaxEnemy(false, [](AEnemy* Enemy) -> float {
			return Enemy->GetDistanceFromCastle();
			}));
		break;
	case EPlaceableAI::FurthestFromCastle:
		SentientOwner->Attack(FindMinMaxEnemy(true, [](AEnemy* Enemy) -> float {
			return Enemy->GetDistanceFromCastle();
			}));
		break;
	case EPlaceableAI::RoundRobin:
		//Make sure its still within bounds
		RobinIndex %= EnemiesInArea.Num();
		SentientOwner->Attack(EnemiesInArea[RobinIndex]);
		RobinIndex = (RobinIndex + 1) % EnemiesInArea.Num();
		break;
	case EPlaceableAI::Random:
		RandomIndex = FMath::Rand() % EnemiesInArea.Num();
		SentientOwner->Attack(EnemiesInArea[RandomIndex]);
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("%s does not have a valid AI set"), *GetName())
	}
}

AEnemy* UDDNormalSentientState::FindMinMaxEnemy(const bool IsMax, std::function<float(AEnemy*)> ValueFunc) const
{
	if (EnemiesInArea.Num() <= 0) {
		UE_LOG(LogTemp, Error, TEXT("Error: EnemiesInArea array is empty"))
		return nullptr;
	}

	AEnemy* ChosenEnemy = EnemiesInArea[0];

	if (!ChosenEnemy) {
		PrintNullPtrError();
		return nullptr;
	}

	float CurrentValue = ValueFunc(ChosenEnemy);

	for (AEnemy* SomeEnemy : EnemiesInArea) {
		if (SomeEnemy) {
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

	return ChosenEnemy;
}
