// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDCraterCreator.h"
#include "Kismet/GameplayStatics.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/Managers/DDCraterManager.h"
#include "DragonsDefense_v2/Characters/DDPlaceable.h"
#include "DragonsDefense_v2/Helpers/DDCrater.h"
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"

ADDCraterCreator::ADDCraterCreator()
{
	DeathCollider = CreateDefaultSubobject<UBoxComponent>("DeathCollider");

	DeathCollider->SetupAttachment(Mesh);

	DeathCollider->SetCollisionObjectType(ECC_EnemyRadiusChannel);
	DDColliderLibrary::SetCollisionChannelToIgnore(DeathCollider, ECC_AttackRadiusChannel);

	DeathCollider->OnComponentBeginOverlap.AddDynamic(this, &ADDCraterCreator::DeathTouchOverlapBegin);
}

void ADDCraterCreator::BeginPlay()
{
	Super::BeginPlay();

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		ADDCraterManager& CraterManager = GameMode->GetCraterManager();
		if (IsValid(&CraterManager)) {
			ADDCrater* Crater = CraterManager.SpawnCrater(CraterClass, GetActorLocation());
			if (Crater) {
				CraterRef = MakeWeakObjectPtr(Crater);
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Warning: %s unable to fetch Crater Manager. This might be an enemy intended for initialization."), *GetName())
		}
	}
	
	FindFloorForCrater();
}

void ADDCraterCreator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CraterRef.IsValid()) {
		ExpandCrater();
	}
}

void ADDCraterCreator::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	AdjustDeathCollider();
}

void ADDCraterCreator::HandleTeleportation()
{
	FindFloorForCrater();
}

void ADDCraterCreator::AdjustDeathCollider()
{
	FVector MeshSize = Mesh->GetStaticMesh()->GetBoundingBox().GetSize();
	FVector Scale = GetActorScale();

	DeathCollider->SetBoxExtent(MeshSize * Scale);
}

void ADDCraterCreator::ExpandCrater()
{
	FVector CurrentPosition = FVector(GetActorLocation().X, GetActorLocation().Y, (StartPosition.Z + ZOffset));

	float Length = (CurrentPosition - StartPosition).Size();

	CraterRef->SetActorLocation((StartPosition + CurrentPosition) / 2.0f);

	FVector InitialScale = CraterRef->GetActorScale3D();
	InitialScale.X = InitialXScale;

	FVector NewScale = InitialScale;
	NewScale.X += Length * ScaleFactor;
	CraterRef->SetActorScale3D(NewScale);
}

void ADDCraterCreator::FindFloorForCrater()
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

	TArray<AActor*> ActorsToIgnore;

	const float DownVectorMultiplication = 10000.0f;
	FHitResult Hit;
	bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		GetActorLocation(),
		GetActorLocation() + (FVector::DownVector * DownVectorMultiplication),
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		Hit,
		true);

	if (bHit) {
		StartPosition = FVector(GetActorLocation().X, GetActorLocation().Y, Hit.Location.Z);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("%s unable to find the floor, spawning Crater on actor position instead..."), *GetName())
		StartPosition = GetActorLocation();
	}
}

void ADDCraterCreator::DeathTouchOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<ADDPlaceable>()) {
		ADDPlaceable* Placeable = Cast<ADDPlaceable>(OtherActor);
		Placeable->UpdateHealth(TouchDamage);
	}
}
