// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDBounceBoundary.h"
#include "Components/BoxComponent.h"
//My classes
#include "DragonsDefense_v2/Characters/DDHorizontalEnemy.h"

// Sets default values
ADDBounceBoundary::ADDBounceBoundary()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Collider = CreateDefaultSubobject<UBoxComponent>("Collider");

	RootComponent = Mesh;
	Collider->SetupAttachment(Mesh);

	Collider->OnComponentBeginOverlap.AddDynamic(this, &ADDBounceBoundary::OverlapBegin);
}

// Called when the game starts or when spawned
void ADDBounceBoundary::BeginPlay()
{
	Super::BeginPlay();
	
	//Might not be necessary
}

void ADDBounceBoundary::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA<ADDHorizontalEnemy>() && OtherActor != RecentlyBounced) {
		ADDHorizontalEnemy* Enemy = Cast<ADDHorizontalEnemy>(OtherActor);
		RecentlyBounced = MakeWeakObjectPtr<ADDHorizontalEnemy>(Enemy);

		//Make sure there isnt a double bounce occuring on a horizontal enemy
		GetWorldTimerManager().SetTimer(BouncedTimer, this, &ADDBounceBoundary::ResetRecentlyBounced, 1.0f, false, 1.0f);

		//Use size of boundary to set random location
		float SizeX = (Mesh->GetStaticMesh()->GetBoundingBox().GetSize().X / 2) * Mesh->GetComponentScale().X;
		//Take absolute location into account
		float LowX = SizeX - GetActorLocation().X;
		float HighX = SizeX + GetActorLocation().X;

		if (Enemy->GetOverrideBounceSpawn()) {
			float Low, High;
			Enemy->GetBounceIntervalSpawn(Low, High);

			Low = FMath::Clamp(Low, -LowX, HighX);
			High = FMath::Clamp(High, -LowX, HighX);

			float RandomX = FMath::RandRange(Low, High);
			Enemy->SetBounceLocation(RandomX);
		}
		else {
			float RandomX = FMath::RandRange(-LowX, HighX);
			Enemy->SetBounceLocation(RandomX);
		}
	}
}

void ADDBounceBoundary::ResetRecentlyBounced()
{
	GetWorldTimerManager().ClearTimer(BouncedTimer);
	RecentlyBounced.Reset();
}

