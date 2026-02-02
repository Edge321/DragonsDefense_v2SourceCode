// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlaceablePreview.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
//My classes
#include "DragonsDefense_v2/Characters/DDPlayer.h"
#include "DragonsDefense_v2/Projectile/DDProjectile.h"
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"
#include "DragonsDefense_v2/DebuffArea/DDDebuffAreaBase.h"
#include "DragonsDefense_v2/Game/Managers/DDTheKing.h"
#include "DragonsDefense_v2/Game/DDGameModeBase.h"

// Sets default values
ADDPlaceablePreview::ADDPlaceablePreview()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Collider = CreateDefaultSubobject<UBoxComponent>("BoxCollider");
	OptionalRadiusMesh = CreateDefaultSubobject<UStaticMeshComponent>("AttackRadius");

	RootComponent = Mesh;
	Collider->SetupAttachment(Mesh);
	OptionalRadiusMesh->SetupAttachment(Mesh);

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	OptionalRadiusMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Collider->SetCollisionObjectType(ECC_PreviewChannel);

	DDColliderLibrary::SetCollisionChannelToIgnore(Collider, ECC_WorldStatic);
	DDColliderLibrary::SetCollisionChannelToIgnore(Collider, ECC_AttackRadiusChannel);
	DDColliderLibrary::SetCollisionChannelToIgnore(Collider, ECC_EnemyRadiusChannel);
}

// Called when the game starts or when spawned
void ADDPlaceablePreview::BeginPlay()
{
	Super::BeginPlay();
	
	GetControllerReference();

	Collider->OnComponentBeginOverlap.AddDynamic(this, &ADDPlaceablePreview::OverlapBegin);
	Collider->OnComponentEndOverlap.AddDynamic(this, &ADDPlaceablePreview::OverlapEnd);

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->OnGameStart.AddDynamic(this, &ADDPlaceablePreview::GameStartEventFunction);
	}
	
	DisableAttackRadius();
}

// Called every frame
void ADDPlaceablePreview::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsHidden()) {
		UpdatePreview();
	}
}

void ADDPlaceablePreview::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Ignore projectiles
	//Ignore debuff areas
	if (OtherActor->IsA<ADDProjectile>() ||
		OtherActor->IsA<ADDDebuffAreaBase>()) {
		return;
	}

	AddActor(OtherActor);
	OnColliding.ExecuteIfBound();
}

void ADDPlaceablePreview::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	RemoveActor(OtherActor);

	if (ActorsColliding.Num() <= 0) {
		OnColliding.ExecuteIfBound();
	}
}

void ADDPlaceablePreview::GameStartEventFunction()
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());

	if (SquaresToIgnore.IsEmpty() && GameMode) {
		SquaresToIgnore = GameMode->GetTheKing().GetOccupationSquares();
	}
}

void ADDPlaceablePreview::RemoveActor(AActor* Actor)
{
	if (Actor) {
		uint32 ID = Actor->GetUniqueID();

		for (AActor* SomeActor : ActorsColliding) {
			if (SomeActor && SomeActor->GetUniqueID() == ID) {
				ActorsColliding.Remove(SomeActor);
				break;
			}
		}
	}
	
}

void ADDPlaceablePreview::AddActor(AActor* Actor)
{
	if (Actor) {
		ActorsColliding.Add(Actor);
	}
}

const FVector ADDPlaceablePreview::GetMeshSize() const
{
	return Mesh->GetStaticMesh()->GetBounds().GetBox().GetSize();
}

void ADDPlaceablePreview::SetMesh(UStaticMesh* NewMesh)
{
	if (NewMesh) {
		Mesh->SetStaticMesh(NewMesh);
		//BoxExtent basically doubles in size from Mesh size
		Collider->SetBoxExtent(GetMeshSize() / 2);
	}
}

void ADDPlaceablePreview::SetScale(FVector Scale)
{
	Mesh->SetWorldScale3D(Scale);
	Mesh->SetRelativeScale3D(Scale);
}

void ADDPlaceablePreview::SetRadiusSize(const float AttackRadius, const float RadiusMeshDiameter)
{
	const float CorrectedRadius = AttackRadius / (RadiusMeshDiameter / 2);
	OptionalRadiusMesh->SetWorldScale3D(FVector(1, CorrectedRadius, CorrectedRadius));
}

void ADDPlaceablePreview::EnableAttackRadius()
{
	OptionalRadiusMesh->SetVisibility(true);
}

void ADDPlaceablePreview::DisableAttackRadius()
{
	OptionalRadiusMesh->SetVisibility(false);
}

void ADDPlaceablePreview::SetMaterial(UMaterialInstance* Material)
{
	int32 Size = OptionalRadiusMesh->GetMaterials().Num();
	int32 MeshMaterialSize = Mesh->GetMaterials().Num();
	
	for (int32 i = 0; i < MeshMaterialSize; i++) {
		Mesh->SetMaterial(i, Material);
	}

	for (int32 i = 0; i < Size; i++) {
		OptionalRadiusMesh->SetMaterial(i, Material);
	}
}

void ADDPlaceablePreview::ClearActorsArray()
{
	ActorsColliding.Empty();
}

void ADDPlaceablePreview::CalculateOffset()
{
	FVector MouseLocation, MouseDirection;

	if (Controller) {
		Controller->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);
	}
	else {
		UE_LOG(LogTemp, Fatal, TEXT("Player controller is null for PlaceablePreview, aborting"))
	}
	//Math for adjusting the mesh to be on top of the floor instead of in it
	//Takes origin at the middle of the shape
	FVector Size = GetMeshSize();
	FVector Scale = Mesh->GetComponentScale();
	float DeltaZ = (Size.Z / 2) * Scale.Z;
	float DeltaX = (DeltaZ * (MouseDirection.X / MouseDirection.Z)) * Scale.X;

	AdjustedOffset = FVector(DeltaX, 0, DeltaZ);
}

const bool ADDPlaceablePreview::GetCurrentlyColliding() const
{
	TArray<AActor*> Actors;
	Collider->GetOverlappingActors(Actors);

	TArray<AActor*> ValidActors;
	for (AActor* Actor : Actors) {
		//Debuff areas dont count alright
		//And projectiles
		//Kinda ugly since this was repeated in Overlap functions but it is what it is
		if (!Actor->IsA<ADDProjectile>() &&
			!Actor->IsA<ADDDebuffAreaBase>()) {
			ValidActors.Add(Actor);
		}
	}

	return ValidActors.Num() > 0 ? true : false;
}

void ADDPlaceablePreview::GetControllerReference()
{
	Controller = UGameplayStatics::GetPlayerController(this, 0);
}

void ADDPlaceablePreview::UpdatePreview()
{
	FVector MouseLocation, MouseDirection;
	FHitResult Hit;
	
	if (Controller) {
		Controller->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);
	}
	else {
		UE_LOG(LogTemp, Fatal, TEXT("Player controller is null for PlaceablePreview, aborting"))
	}

	//The big multiplication is to make sure to go far out to almost guarantee a hit from LineTrace
	FVector End = (MouseDirection * 10000) + MouseLocation;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	
	//Lots of things happening here, so
	//Basically just returns a FHitResult on the object the line trace collides with first
	bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(
		this, 
		MouseLocation, 
		End, 
		ObjectTypes, 
		false, 
		SquaresToIgnore, 
		EDrawDebugTrace::None, 
		Hit, 
		true);

	if (bHit) {
		SetActorLocation(Hit.Location + AdjustedOffset);
	}
}
