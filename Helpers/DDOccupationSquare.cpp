// Copyright (c) 2025, Edge Cope Corp. All rights reserved

#include "DDOccupationSquare.h"
#include "Components/BoxComponent.h"
//My classes
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"
#include "DragonsDefense_v2/Characters/DDPlaceable.h"
#include "DragonsDefense_v2/Characters/Enemy.h"

// Sets default values
ADDOccupationSquare::ADDOccupationSquare()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Collider = CreateDefaultSubobject<UBoxComponent>("Collider");

	RootComponent = Mesh;

	Collider->SetupAttachment(Mesh);

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Collider->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	Collider->OnComponentBeginOverlap.AddDynamic(this, &ADDOccupationSquare::OverlapBegin);
	Collider->OnComponentEndOverlap.AddDynamic(this, &ADDOccupationSquare::OverlapEnd);

	DDColliderLibrary::SetCollisionChannelToIgnore(Collider, ECC_AttackRadiusChannel);
	//DDColliderLibrary::SetCollisionChannelToIgnore(Collider, ECC_EnemyChannel);
	DDColliderLibrary::SetCollisionChannelToIgnore(Collider, ECC_EnemyRadiusChannel);
}

void ADDOccupationSquare::BeginPlay()
{
	Super::BeginPlay();

#if UE_GAME
	Collider->bHiddenInGame = true;
	Collider->SetVisibility(false);
#endif

}

void ADDOccupationSquare::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<AEnemy>()) {
		TWeakObjectPtr<AEnemy> Enemy = MakeWeakObjectPtr(Cast<AEnemy>(OtherActor));
		if (Enemy.IsValid() && !EnemyDeathEventsMap.Contains(Enemy)) {
			FDelegateHandle Handler = Enemy->OnActorDeath.AddUObject(this, &ADDOccupationSquare::EventFunctionDetectEnemyDeath);
			EnemyDeathEventsMap.Add(Enemy, Handler);
		}
	}
	else if (OtherActor->IsA<ADDPlaceable>()) {
		TWeakObjectPtr<ADDPlaceable> Placeable = MakeWeakObjectPtr(Cast<ADDPlaceable>(OtherActor));
		if (Placeable.IsValid() && !PlaceableMap.Contains(Placeable)) {
			float OldThreatLevel = ThreatLevel;
			float ActualThreatLevel = Placeable->GetThreatLevel() * ThreatLevelMultiplier;
			ThreatLevel += ActualThreatLevel;
			AdjustSecretStatsOnPlaceable(Placeable);
			Placeable->OnPlaceableDeath.AddUObject(this, &ADDOccupationSquare::OnPlaceableDeath);
			PlaceableMap.Add(Placeable, ActualThreatLevel);
			OnThreatLevelChange.ExecuteIfBound(SquareCoordinate, OldThreatLevel);
		}
	}
}

void ADDOccupationSquare::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA<AEnemy>()) {
		//No need to listen to the event anymore if the enemy isn't in the square
		TWeakObjectPtr<AEnemy> Enemy = MakeWeakObjectPtr(Cast<AEnemy>(OtherActor));
		FDelegateHandle Handler;
		if (EnemyDeathEventsMap.RemoveAndCopyValue(Enemy, Handler)) {
			Enemy->OnActorDeath.Remove(Handler);
		}
	}
}

void ADDOccupationSquare::InitializeOccupationSquare(const FVector2D Size, const FVector2D Coordinate)
{
	FVector MeshSize = Mesh->GetStaticMesh()->GetBoundingBox().GetSize();
	FVector Scale = FVector(Size, MeshSize.Z) / MeshSize;

	Mesh->SetWorldScale3D(Scale);
	Collider->SetWorldScale3D(Mesh->GetComponentScale());
	SquareCoordinate = Coordinate;
}

void ADDOccupationSquare::FetchSideCoordinates()
{
	FVector MeshSize = Mesh->GetStaticMesh()->GetBoundingBox().GetSize();
	FVector Scale = GetActorScale3D();

	//Assuming location is the middle of the square
	FVector Location = GetActorLocation();
	float HalfSizeSquare = (MeshSize.Y * Scale.Y) / 2.0f;
	LeftSideCoordinate = Location.Y - HalfSizeSquare;
	RightSideCoordinate = Location.Y + HalfSizeSquare;
}

void ADDOccupationSquare::FetchOccupationStats(const float ThreatLevelMultiplierScaling)
{
	TArray<AActor*> OverlappingActors;
	float ActualThreatLevel;

	ThreatLevelMultiplier = ThreatLevelMultiplierScaling;

	Collider->GetOverlappingActors(OverlappingActors, ADDPlaceable::StaticClass());

	PlaceableAmount = OverlappingActors.Num();

	for (AActor* Actor : OverlappingActors) {
		ADDPlaceable* Placeable = Cast<ADDPlaceable>(Actor);

		TWeakObjectPtr<ADDPlaceable> WeakPlaceable = MakeWeakObjectPtr(Placeable);
		if (!PlaceableMap.Contains(WeakPlaceable)) {
			ActualThreatLevel = WeakPlaceable->GetThreatLevel() * ThreatLevelMultiplier;
			ThreatLevel += ActualThreatLevel;
			AdjustSecretStatsOnPlaceable(WeakPlaceable);
			PlaceableMap.Add(WeakPlaceable, ActualThreatLevel);
			WeakPlaceable->OnPlaceableDeath.AddUObject(this, &ADDOccupationSquare::OnPlaceableDeath);
		}
	}
}

void ADDOccupationSquare::ResetOccupationSquare()
{
	ThreatLevel = 0;
	PlaceableAmount = 0;
	PlaceableMap.Empty();
}

void ADDOccupationSquare::OnPlaceableDeath(ADDPlaceable* Placeable)
{
	float PlaceableThreatLevel;
	float OldThreatLevel;

	TWeakObjectPtr<ADDPlaceable> WeakPlaceable = MakeWeakObjectPtr(Placeable);
	if (!PlaceableMap.Contains(WeakPlaceable)) {
		UE_LOG(LogTemp, Error, TEXT("%s's PlaceableMap does not contain %s. Doing nothing..."), *GetName(), *(WeakPlaceable->GetName()));
		return;
	}

	PlaceableMap.RemoveAndCopyValue(WeakPlaceable, PlaceableThreatLevel);

	OldThreatLevel = ThreatLevel;
	ThreatLevel -= PlaceableThreatLevel;
	OnThreatLevelChange.ExecuteIfBound(SquareCoordinate, OldThreatLevel);
}

void ADDOccupationSquare::EventFunctionDetectEnemyDeath(AEnemy* Enemy) const
{
	OnDetectEnemyDeath.ExecuteIfBound(SquareCoordinate, Enemy);
}

void ADDOccupationSquare::SetSecretStatsBases(const float Offense, const float Defense, const float Support)
{
	OffenseBase = Offense;
	DefenseBase = Defense;
	SupportBase = Support;
}

void ADDOccupationSquare::SetSecretStatsScaling(const float Offense, const float Defense, const float Support)
{
	OffenseScaling = Offense;
	DefenseScaling = Defense;
	SupportScaling = Support;
}

const float ADDOccupationSquare::GetThreatLevel() const
{
	return ThreatLevel;
}

const FVector2D ADDOccupationSquare::GetSideCoordinates() const
{
	return FVector2D(LeftSideCoordinate, RightSideCoordinate);
}

void ADDOccupationSquare::AdjustSecretStatsOnPlaceable(TWeakObjectPtr<ADDPlaceable> Placeable) const
{
	FDDSecretStats SecretStats;
	SecretStats.Offense = FMathf::Clamp(OffenseBase + (OffenseScaling * SquareCoordinate.X), 0.0f, INFINITY);
	SecretStats.Defense = FMathf::Clamp(DefenseBase + (DefenseScaling * SquareCoordinate.X), 0.0f, INFINITY);
	SecretStats.Support = FMathf::Clamp(SupportBase + (SupportScaling * SquareCoordinate.X), 0.0f, INFINITY);

	Placeable->UpdateSecretStats(SecretStats);
}

