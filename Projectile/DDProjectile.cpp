// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#include "DDProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraComponent.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/DDProjectileManager.h"
#include "DragonsDefense_v2/Characters/DDRetaliatorSentient.h"
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"

// Sets default values
ADDProjectile::ADDProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Collider = CreateDefaultSubobject<UBoxComponent>("BoxCollider");
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileEffects = CreateDefaultSubobject<UNiagaraComponent>("ProjectileEffects");

	RootComponent = Mesh;
	Collider->SetupAttachment(Mesh);
	ProjectileEffects->SetupAttachment(Mesh);

	//This is to make sure only the collider is allowed to collide
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement->Velocity = FVector(800.0f, 0, 0); //Default velocity of projectile 

	ProjectileMovement->ProjectileGravityScale = 0;
}

// Called when the game starts or when spawned
void ADDProjectile::BeginPlay()
{
	Super::BeginPlay();

	FVector TempLocation = GetActorLocation();
	SetActorLocation(FVector(TempLocation.X, TempLocation.Y, ZLocation));
	
	//Prevents collider from colliding if spawned inside its owner (heehee)
	Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	Collider->OnComponentBeginOverlap.AddDynamic(this, &ADDProjectile::OverlapBegin);

	LastPosition = GetActorLocation();

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		ADDProjectileManager& ProjectileManager = GameMode->GetProjectileManager();
		ProjectileManager.AddProjectileToPool(this);
	}

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ADDProjectile::EnableCollision, 0.05f, false);
	GetWorldTimerManager().SetTimer(ProjectileTimer, this, &ADDProjectile::DestroySelf, ProjectileLifetime, false);
}

void ADDProjectile::SetVelocity(const FVector Velocity)
{
	ProjectileMovement->Velocity = Velocity;
}

void ADDProjectile::SetDestination(const FVector NewDestination, const float TimeTolerance)
{
	if (!bOverrideProjectileTime) {
		float TimeToMeetDestination = 0;
		//Using velocity formula to solve for time
		FVector Displacement = NewDestination - GetActorLocation();
		//Turn vectors into magnitude of vector to get a scalar result
		float Speed = ProjectileMovement->Velocity.Size();
		if (!FMath::IsNearlyZero(Speed)) {
			TimeToMeetDestination = Displacement.Size() / Speed;
			TimeToMeetDestination += TimeTolerance;
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("%s destroyed for being close to 0 velocity. Avoids dividing by 0"), *GetName())
			DestroySelf();
		}

		GetWorldTimerManager().SetTimer(DestinationTimer, this, &ADDProjectile::DestroySelf, TimeToMeetDestination, false);
	}
}

void ADDProjectile::SetSpecificTarget(ALivingActor* Living)
{
	SpecificTarget = MakeWeakObjectPtr(Living);
}

const UStaticMesh* ADDProjectile::GetStaticMesh() const
{
	return Mesh->GetStaticMesh();
}

void ADDProjectile::ForceDestroyProjectile()
{
	DestroySelf();
}

void ADDProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckPossibleCollisionPath();
}

const float ADDProjectile::GetDamage() const
{
	return ProjectileDamage;
}

const FVector ADDProjectile::GetProjectileVelocity() const
{
	return ProjectileMovement->Velocity;
}

void ADDProjectile::SetDamage(float Damage)
{
	ProjectileDamage = Damage;
}

void ADDProjectile::SetProjectileOwner(const AActor* Actor)
{
	ActorOwner = Actor;
}

const AActor* ADDProjectile::GetProjectileOwner() const
{
	return ActorOwner;
}

void ADDProjectile::SetCollisionChannelToIgnore(const ECollisionChannel Channel)
{
	Collider->SetCollisionResponseToChannel(Channel, ECR_Ignore);
	ChannelsIgnored.Add(Channel);
}

void ADDProjectile::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ALivingActor* Living = Cast<ALivingActor>(OtherActor);

	//Checking if valid pointer and actor isn't hurt from own projectile
	if (Living && Living != ActorOwner) {
		if (SpecificTarget.IsValid() && SpecificTarget == Living) {
			HandleHit(SpecificTarget.Get());
		}
		else {
			if (Living->IsA<ADDRetaliatorSentient>()) {
				ADDRetaliatorSentient* Retaliator = Cast<ADDRetaliatorSentient>(Living);
				Retaliator->HandleBeingHit(this);
			}
			HandleHit(Living);
		}
	}
}

void ADDProjectile::EnableCollision()
{
	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ADDProjectile::DestroySelf()
{
	OnProjectileDestroyed.ExecuteIfBound(this);
}

void ADDProjectile::CheckPossibleCollisionPath()
{
	FHitResult Hit;
	
	FCollisionObjectQueryParams QueryParams;
	FCollisionQueryParams Params;

	Params.AddIgnoredActor(this);
	Params.AddIgnoredActor(ActorOwner);

	QueryParams.AddObjectTypesToQuery(ECC_PlaceableChannel);
	QueryParams.AddObjectTypesToQuery(ECC_EnemyChannel);

	//Remove channels the projectile is set to ignoring
	for (ECollisionChannel Channel : ChannelsIgnored) {
		QueryParams.RemoveObjectTypesToQuery(Channel);
	}

	UWorld* World = GetWorld();
	if (World) {
		World->LineTraceSingleByObjectType(Hit, LastPosition, GetActorLocation(), QueryParams, Params);	
	}

	ALivingActor* Living = Cast<ALivingActor>(Hit.GetActor());
	if (Living) {
		HandleHit(Living);
		//UE_LOG(LogTemp, Log, TEXT("Hit something that should've been hit!"))
	}

	LastPosition = GetActorLocation();
}

void ADDProjectile::HandleHit(ALivingActor* Living)
{
	Living->UpdateHealth(ProjectileDamage);
	DestroySelf();
}
