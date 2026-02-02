// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDTinkererEnemy.h"
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"
#include "DragonsDefense_v2/Characters/DDPlaceable.h"

ADDTinkererEnemy::ADDTinkererEnemy()
{
	ConvertCollider = CreateDefaultSubobject<UBoxComponent>("ConvertCollider");

	ConvertCollider->SetupAttachment(Mesh);
	ConvertCollider->SetCollisionObjectType(ECC_EnemyRadiusChannel);
	DDColliderLibrary::SetCollisionChannelToIgnore(ConvertCollider, ECC_EnemyRadiusChannel);
	DDColliderLibrary::SetCollisionChannelToIgnore(ConvertCollider, ECC_AttackRadiusChannel);

	ConvertCollider->OnComponentBeginOverlap.AddDynamic(this, &ADDTinkererEnemy::OverlapConvertBegin);
}

void ADDTinkererEnemy::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FVector Origin, BoxExtent;

	Mesh->GetLocalBounds(Origin, BoxExtent);
	FVector NewBox(ConvertSize.X, ConvertSize.Y, BoxExtent.Z);
	ConvertCollider->SetBoxExtent(NewBox);
}

void ADDTinkererEnemy::OverlapConvertBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<ADDPlaceable>()) {
		ADDPlaceable* Placeable = Cast<ADDPlaceable>(OtherActor);
		Placeable->ConvertToEvil();
	}
}
