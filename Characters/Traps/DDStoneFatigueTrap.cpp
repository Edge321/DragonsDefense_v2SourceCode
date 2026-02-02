// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDStoneFatigueTrap.h"
#include "Components/SphereComponent.h"
//My classes
#include "DragonsDefense_v2/DebuffArea/DDDebuffAreaBase.h"
#include "DragonsDefense_v2/Game/Managers/DDDebuffAreaManager.h"
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"
#include "DragonsDefense_v2/Characters/Enemy.h"

ADDStoneFatigueTrap::ADDStoneFatigueTrap()
{
	DebuffRadiusMesh = CreateDefaultSubobject<UStaticMeshComponent>("DebuffRadiusMesh");
	DebuffRadiusCollider = CreateDefaultSubobject<USphereComponent>("DebuffRadiusCollider");

	DebuffRadiusMesh->SetupAttachment(Mesh);
	DebuffRadiusCollider->SetupAttachment(Mesh);

	DebuffRadiusMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DebuffRadiusCollider->SetCollisionObjectType(ECC_AttackRadiusChannel);
	DDColliderLibrary::SetCollisionChannelToIgnore(DebuffRadiusCollider, ECC_AttackRadiusChannel);
	DDColliderLibrary::SetCollisionChannelToIgnore(DebuffRadiusCollider, ECC_EnemyRadiusChannel);
	DDColliderLibrary::SetCollisionChannelToIgnore(DebuffRadiusCollider, ECC_PreviewChannel);
	DDColliderLibrary::SetCollisionChannelToIgnore(DebuffRadiusCollider, ECC_PlaceableChannel);
}

void ADDStoneFatigueTrap::BeginPlay()
{
	Super::BeginPlay();

	//Get rid of these bozos, its the DebuffRadiusCollider's turn now
	Collider->OnComponentBeginOverlap.Clear();
	Collider->OnComponentEndOverlap.Clear();

	TArray<AActor*> Actors;
	DebuffRadiusCollider->GetOverlappingActors(Actors);
	OnSpawnOverlap(Actors);

	DebuffRadiusCollider->OnComponentBeginOverlap.AddDynamic(this, &ADDStoneFatigueTrap::OverlapBegin);
	DebuffRadiusCollider->OnComponentEndOverlap.AddDynamic(this, &ADDStoneFatigueTrap::OverlapEnd);

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameModeRef = MakeWeakObjectPtr(GameMode);
	}
}

void ADDStoneFatigueTrap::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (DebuffRadiusCollider) {
		DebuffRadiusCollider->SetSphereRadius(DebuffDetectionRadius);
	}

	FVector OriginalMeshSize = GetRadiusMeshSize();
	float CorrectedScale = DebuffDetectionRadius * 2;
	//Scales in Y, Z to avoid making circle thicker
	FVector Scale(1, CorrectedScale / OriginalMeshSize.Y, CorrectedScale / OriginalMeshSize.Z);

	if (DebuffRadiusMesh) {
		DebuffRadiusMesh->SetRelativeScale3D(Scale);
	}
}

void ADDStoneFatigueTrap::HandleBeginOverlap(AActor* Actor)
{
	if (Actor->IsA<AEnemy>()) {
		AEnemy* Enemy = Cast<AEnemy>(Actor);
		EnemiesInArea.Add(MakeWeakObjectPtr(Enemy));
		BeginDebuffSpawn();
	}
}

void ADDStoneFatigueTrap::HandleEndOverlap(AActor* Actor)
{
	if (Actor->IsA<AEnemy>()) {
		AEnemy* Enemy = Cast<AEnemy>(Actor);
		EnemiesInArea.RemoveSwap(Enemy);
		if (EnemiesInArea.IsEmpty()) {
			StopDebuffSpawn();
		}
	}
}

void ADDStoneFatigueTrap::BeginDebuffSpawn()
{
	if (!bIsSpawningDebuffs) {
		bIsSpawningDebuffs = true;
		GetWorldTimerManager().SetTimer(SpawnDebuffTimer, this, &ADDStoneFatigueTrap::SpawnDebuffArea, TimeToSpawnSlowness, true, 0);
	}
}

void ADDStoneFatigueTrap::StopDebuffSpawn()
{
	if (bIsSpawningDebuffs) {
		bIsSpawningDebuffs = false;
		GetWorldTimerManager().ClearTimer(SpawnDebuffTimer);
	}
}

void ADDStoneFatigueTrap::SpawnDebuffArea()
{
	ADDDebuffAreaBase* Debuff = GameModeRef->GetDebuffAreaManager().SpawnDebuffArea(DebuffAreaClass, 
																				GetActorLocation() + DebuffAreaOffset, 
																				FRotator::ZeroRotator, 
																				true);
	if (Debuff) {
		Debuff->SetAreaOwner(this);
		Debuff->SetDebuffAmount(SlownessMultiplier);
		Debuff->SetCollisionChannelToIgnore(ECC_PlaceableChannel);
	}
}

const FVector ADDStoneFatigueTrap::GetRadiusMeshSize() const
{
	if (DebuffRadiusMesh && DebuffRadiusMesh->GetStaticMesh()) {
		return DebuffRadiusMesh->GetStaticMesh()->GetBounds().GetBox().GetSize();
	}
	else {
		return FVector::One();
	}
}
