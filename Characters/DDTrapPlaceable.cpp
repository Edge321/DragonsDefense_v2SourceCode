// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDTrapPlaceable.h"
//My classes
#include "DragonsDefense_v2/Characters/Enemy.h"
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"

// Sets default values
ADDTrapPlaceable::ADDTrapPlaceable()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");

	RootComponent = Mesh;
	Collider->SetupAttachment(Mesh);
	Arrow->SetupAttachment(Mesh);

	//He was forced to use only collider box
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Collider->OnComponentBeginOverlap.AddDynamic(this, &ADDTrapPlaceable::OverlapBegin);
	Collider->OnComponentEndOverlap.AddDynamic(this, &ADDTrapPlaceable::OverlapEnd);
	//DDColliderLibrary::SetCollisionChannelToIgnore(Collider, ECC_AttackRadiusChannel); //BUG - Problem with not being targeted by evil sentients. Maybe intentional bug tho?

	Mesh->bRenderCustomDepth = true;
}

// Called when the game starts or when spawned
void ADDTrapPlaceable::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> OverlapActors;
	Collider->GetOverlappingActors(OverlapActors);
	OnSpawnOverlap(OverlapActors);

	if (bIgnoreProjectiles) {
		//It looks like only projectiles are in ECC_WorldDynamic
		DDColliderLibrary::SetCollisionChannelToIgnore(Collider, ECC_WorldDynamic);
	}
}

void ADDTrapPlaceable::ConvertToEvil()
{
	if (!bIsEvil) {
		bIsEvil = true;
		SetSellingPrice(0);
		AddChannelHurtByExplosion(ECC_PlaceableChannel);
		OnTurnEvil.ExecuteIfBound(this);
		OnExplode();
	}
}

void ADDTrapPlaceable::OnExplode()
{
	//Spawn a big ass sphere that kills everything in the vicinity, then itself
	GetWorldTimerManager().SetTimer(ExplodeTimer, this, &ADDTrapPlaceable::Explode, SecondsTilExplode);
}

void ADDTrapPlaceable::AddChannelHurtByExplosion(ECollisionChannel Channel)
{
	ChannelsForExplosion.Add(Channel);
}

void ADDTrapPlaceable::RemoveChannelHurtByExplosion(ECollisionChannel Channel)
{
	ChannelsForExplosion.Remove(Channel);
}

const UStaticMeshComponent* ADDTrapPlaceable::GetMesh() const
{
	return Mesh;
}

void ADDTrapPlaceable::EnableHighlight()
{
	Mesh->SetCustomDepthStencilValue(Stencil_HighlightDepth);
}

void ADDTrapPlaceable::DisableHighlight()
{
	Mesh->SetCustomDepthStencilValue(0);
}

void ADDTrapPlaceable::OnSpawnOverlap(TArray<AActor*> OverlapActors)
{
	for (AActor* Actor : OverlapActors) {
		if (Actor && Actor->IsA<AEnemy>()) {
			HandleBeginOverlap(Actor);
		}
	}
}

void ADDTrapPlaceable::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) 
{
	if (!bIsEvil && OtherActor && !OtherComp->ComponentHasTag(ENEMY_COLLIDER_TAG)) {
		if (OtherActor->IsA<AEnemy>()) {
			AEnemy* Enemy = Cast<AEnemy>(OtherActor);
			if (Enemy->GetIsTrapDetectable()) {
				HandleBeginOverlap(OtherActor);
			}
		}
	}
}

void ADDTrapPlaceable::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	HandleEndOverlap(OtherActor);
}

void ADDTrapPlaceable::Explode()
{
	TArray<AActor*> IgnoreActors;
	TArray<FOverlapResult> Overlaps;
	FCollisionQueryParams SphereParams(TEXT("TrapExplosion"), false, this);

	FCollisionObjectQueryParams QueryParams;

	for (ECollisionChannel Channel : ChannelsForExplosion) {
		QueryParams.AddObjectTypesToQuery(Channel);
	}

	UWorld* World = GetWorld();
	if (World)
	{
		World->OverlapMultiByObjectType(
			Overlaps, 
			GetActorLocation(), 
			FQuat::Identity, 
			QueryParams,
			FCollisionShape::MakeSphere(ExplosionRadius),
			SphereParams);

		DrawDebugSphere(World, GetActorLocation(), ExplosionRadius, 12, FColor::Red, false, 5.0f, 0, 2);
	}

	for (FOverlapResult Result : Overlaps) {
		ALivingActor* LivingActor = Cast<ALivingActor>(Result.GetActor());
		if (LivingActor && LivingActor != this) {
			LivingActor->UpdateHealth(-ExplosionDamage);
		}
	}

	OnDeath();
}

void ADDTrapPlaceable::HandleBeginOverlap(AActor* Actor)
{
	if (Actor->IsA<AEnemy>()) {
		AEnemy* Enemy = Cast<AEnemy>(Actor);
		if (Enemy->GetIsTrapDetectable()) {
			Enemy->UpdateHealth(GetDamage());
			OnPlaceableDeath.Broadcast(this);
		}
	}
}

void ADDTrapPlaceable::HandleEndOverlap(AActor* Actor)
{
	//Nothing occurs
}
