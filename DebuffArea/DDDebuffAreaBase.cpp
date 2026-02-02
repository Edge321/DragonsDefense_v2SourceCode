// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDDebuffAreaBase.h"
#include "Components/BoxComponent.h"
//My classes
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"

// Sets default values
ADDDebuffAreaBase::ADDDebuffAreaBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Collider = CreateDefaultSubobject<UBoxComponent>("Collider");

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RootComponent = Mesh;
	Collider->SetupAttachment(Mesh);

	Collider->OnComponentBeginOverlap.AddDynamic(this, &ADDDebuffAreaBase::OverlapAreaBegin);
	Collider->OnComponentEndOverlap.AddDynamic(this, &ADDDebuffAreaBase::OverlapAreaEnd);

	SetCollisionChannelToIgnore(ECC_EnemyRadiusChannel);
	SetCollisionChannelToIgnore(ECC_AttackRadiusChannel);
}

// Called when the game starts or when spawned
void ADDDebuffAreaBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (!bIsImmortal) {
		GetWorldTimerManager().SetTimer(LifetimeTimer, this, &ADDDebuffAreaBase::DebuffAreaDeath, LifeTime, false);
	}
	if (bIsEnabledAtBeginning) {
		Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Make sure owner of DebuffArea isn't hurt by own thing
		GetWorldTimerManager().SetTimer(NoCollisionTimer, this, &ADDDebuffAreaBase::EnableCollision, 0.1f, false);
	}
	else {
		DisableDebuffArea();
	}

	CheckDebuffClass();
}

const FVector ADDDebuffAreaBase::GetActualMeshSize() const
{
	FVector MeshSize = Mesh->GetStaticMesh()->GetBoundingBox().GetSize();
	FVector ActualSize = MeshSize * GetActorScale3D();
	return ActualSize;
}

void ADDDebuffAreaBase::SetAreaOwner(AActor* NewAreaOwner)
{
	if (NewAreaOwner) {
		AreaOwner = MakeWeakObjectPtr(NewAreaOwner);
	}
}

void ADDDebuffAreaBase::SetCollisionChannelToIgnore(const ECollisionChannel Channel)
{
	Collider->SetCollisionResponseToChannel(Channel, ECR_Ignore);
}

void ADDDebuffAreaBase::SetDebuffAmount(const float NewDebuffAmount)
{
	DebuffAmount = NewDebuffAmount;
}

void ADDDebuffAreaBase::EnableDebuffArea()
{
	Mesh->SetHiddenInGame(false);
	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ADDDebuffAreaBase::DisableDebuffArea()
{
	Mesh->SetHiddenInGame(true);
	Collider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADDDebuffAreaBase::OverlapAreaBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA<ALivingActor>()) {
		if (AreaOwner.IsValid()) {
			//Gotta check if there is an owner here
			if (OtherActor != AreaOwner) {
				ALivingActor* Living = Cast<ALivingActor>(OtherActor);
				AddDebuffPair(Living);
			}
		}
		else {
			ALivingActor* Living = Cast<ALivingActor>(OtherActor);
			AddDebuffPair(Living);
		}
	}
}

void ADDDebuffAreaBase::OverlapAreaEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (bCanRemoveDebuff && OtherActor && OtherActor->IsA<ALivingActor>() && OtherActor != AreaOwner) {
		ALivingActor* Living = Cast<ALivingActor>(OtherActor);
		RemoveDebuffPair(Living);
	}
}

void ADDDebuffAreaBase::CheckDebuffClass() const
{
	check(DebuffClass)
}

void ADDDebuffAreaBase::AddDebuffPair(ALivingActor* Living)
{
	FDebuffPair DebuffPair;
	
	if (Living) {
		UDDDebuffBase* Debuff = NewObject<UDDDebuffBase>(this, DebuffClass);
		Debuff->SetDebuffAmount(DebuffAmount);
		Living->AddDebuff(Debuff);
		
		DebuffPair.Debuff = MakeWeakObjectPtr(Debuff);
		DebuffPair.Living = MakeWeakObjectPtr(Living);
		PairArray.Add(DebuffPair);
	}
}

void ADDDebuffAreaBase::DebuffAreaDeath()
{
	OnDeath.ExecuteIfBound(this);
}

void ADDDebuffAreaBase::EnableCollision()
{
	Collider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

