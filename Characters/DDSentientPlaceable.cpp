// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDSentientPlaceable.h"
#include "Components/SphereComponent.h"
//My classes
#include "DragonsDefense_v2/Characters/DDPlaceableAI.h"
#include "DragonsDefense_v2/Characters/PlaceableStates/DDNormalSentientState.h"
#include "DragonsDefense_v2/Characters/PlaceableStates/DDEvilSentientState.h"
#include "DragonsDefense_v2/Characters/DDShrewdEnemy.h"
#include "DragonsDefense_v2/Characters/DDMageSniperEnemy.h"
#include "DragonsDefense_v2/Projectile/DDProjectile.h"
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"

ADDSentientPlaceable::ADDSentientPlaceable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	AttackCollider = CreateDefaultSubobject<USphereComponent>("AttackCollider");
	RadiusMesh = CreateDefaultSubobject<UStaticMeshComponent>("AttackRadius");

	RootComponent = Mesh;
	Collider->SetupAttachment(Mesh);
	AttackCollider->SetupAttachment(Mesh);
	Arrow->SetupAttachment(Mesh);
	RadiusMesh->SetupAttachment(Mesh);

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RadiusMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackCollider->SetCollisionObjectType(ECC_AttackRadiusChannel);
	DDColliderLibrary::SetCollisionChannelToIgnore(AttackCollider, ECC_PreviewChannel);
	DDColliderLibrary::SetCollisionChannelToIgnore(AttackCollider, ECC_AttackRadiusChannel);

	AttackCollider->OnComponentBeginOverlap.AddDynamic(this, &ADDSentientPlaceable::OverlapBegin);
	AttackCollider->OnComponentEndOverlap.AddDynamic(this, &ADDSentientPlaceable::OverlapEnd);

	Mesh->bRenderCustomDepth = true;

	CurrentAI = EPlaceableAI::ClosestToCastle;
}

void ADDSentientPlaceable::BeginPlay()
{
	Super::BeginPlay();

	InitializeState();

	ActualAttackRadius = AttackRadius;
	ActualAttackSpeed = AttackSpeed;
	ActualInaccuracy = Inaccuracy;
	TempPrice = ActualPrice;

	HideAttackRadius();
}

const UStaticMeshComponent* ADDSentientPlaceable::GetMesh() const
{
	return Mesh;
}

const float ADDSentientPlaceable::GetAttackRadius() const
{
	return AttackRadius;
}

const FVector ADDSentientPlaceable::GetRadiusMeshSize() const
{
	if (RadiusMesh && RadiusMesh->GetStaticMesh()) {
		return RadiusMesh->GetStaticMesh()->GetBounds().GetBox().GetSize();
	}
	else {
		return FVector::One();
	}
}

void ADDSentientPlaceable::SetAttackRadiusMod(const float AttackRadiusMod)
{
	AttackRadius = ActualAttackRadius * AttackRadiusMod;

	AttackCollider->SetSphereRadius(AttackRadius);
	AdjustAttackMesh();
}

void ADDSentientPlaceable::SetAttackSpeedMod(const float AttackSpeedMod)
{
	AttackSpeed = ActualAttackSpeed * AttackSpeedMod;
}

void ADDSentientPlaceable::SetAccuracyMod(const float InaccuracyMod)
{
	float TempAccuracy = FMath::Abs(InaccuracyMod);
	Inaccuracy = ActualInaccuracy + TempAccuracy;
}

void ADDSentientPlaceable::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (PlaceableData) {
		PlaceableData->AttackSpeed = AttackSpeed;
		PlaceableData->AttackRadius = AttackRadius;
	}

	//Adjusting Attack Collider and Attack Mesh as size of AttackRadius
	if (AttackCollider) {
		AttackCollider->SetSphereRadius(AttackRadius);
	}
	AdjustAttackMesh();
}

void ADDSentientPlaceable::SetAI(const EPlaceableAI AIState)
{
	CurrentAI = AIState;
}

void ADDSentientPlaceable::InitializeProjectile(ADDProjectile* Proj, const float CustomDamage) const
{
	if (CustomDamage >= 0) {
		Proj->SetDamage(Damage); //Can't set CustomDamage = Damage in the argument, I do this instead
	}
	else {
		Proj->SetDamage(CustomDamage);
	}
}

void ADDSentientPlaceable::ConvertToEvil()
{
	if (!PlaceableState.GetObject()->IsA<UDDEvilSentientState>()) {
		bIsEvil = true;
		UDDEvilSentientState* EvilState = NewObject<UDDEvilSentientState>(this);
		SetPlaceableState(EvilState);
	}
}

EPlaceableAI ADDSentientPlaceable::GetAI() const
{
	return CurrentAI;
}

void ADDSentientPlaceable::ShowAttackRadius()
{
	RadiusMesh->SetVisibility(true);
}

void ADDSentientPlaceable::HideAttackRadius()
{
	RadiusMesh->SetVisibility(false);
}

void ADDSentientPlaceable::EnableHighlight()
{
	Mesh->SetCustomDepthStencilValue(Stencil_HighlightDepth);
}

void ADDSentientPlaceable::DisableHighlight()
{
	Mesh->SetCustomDepthStencilValue(0);
}

void ADDSentientPlaceable::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA<ADDMageSniperEnemy>()) {
		ADDMageSniperEnemy* Mage = Cast<ADDMageSniperEnemy>(OtherActor);
		Mage->OnTurnDetectable.AddUObject(this, &ADDSentientPlaceable::SniperTurnDetectable);
	}
	PlaceableState->HandleBeginOverlap(OtherActor);
}

void ADDSentientPlaceable::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA<ADDMageSniperEnemy>()) {
		ADDMageSniperEnemy* Mage = Cast<ADDMageSniperEnemy>(OtherActor);
	}
	PlaceableState->HandleEndOverlap(OtherActor);
}

void ADDSentientPlaceable::SniperTurnDetectable(ADDMageSniperEnemy* Sniper) {
	if (AttackCollider->IsOverlappingActor(Sniper)) {
		PlaceableState->HandleBeginOverlap(Sniper);
	}
}

TArray<AActor*> ADDSentientPlaceable::GetAllActorsInRadius() const
{
	TArray<AActor*> TempActors;
	AttackCollider->GetOverlappingActors(TempActors);
	return TempActors;
}

void ADDSentientPlaceable::ValidateProjectile()
{
	check(Projectile != nullptr)
}

void ADDSentientPlaceable::StartAttack()
{
	bIsAttacking = true;
	if (!GetWorldTimerManager().IsTimerActive(AttackHandle)) {
		GetWorldTimerManager().SetTimer(AttackHandle, this, &ADDSentientPlaceable::TriggerAttack, AttackSpeed, true, 0.0f);
	}
}

void ADDSentientPlaceable::StopAttack()
{
	bIsAttacking = false;
}

void ADDSentientPlaceable::Attack(AEnemy* Enemy) const
{
	float OriginalAccuracy = 1.0f;
	ADDProjectile* Proj = GetWorld()->SpawnActor<ADDProjectile>(Projectile, GetActorLocation(), GetActorRotation());
	if (Proj) {
		Proj->SetProjectileOwner(this);
		Proj->SetCollisionChannelToIgnore(ECC_PlaceableChannel);
		Proj->SetCollisionChannelToIgnore(ECC_AttackRadiusChannel);
		Proj->SetCollisionChannelToIgnore(ECC_EnemyRadiusChannel);
		InitializeProjectile(Proj, 1.0f);
		
		const FVector EnemyLocation = Enemy->GetActorLocation();
		const FVector EnemyVelocity = Enemy->GetFloatingPawnMovement()->Velocity;

		//Calculating inaccuracy, if any
		float RandomAccuracy = FMath::RandRange(OriginalAccuracy - Inaccuracy, OriginalAccuracy + Inaccuracy);
		//Takes possible innacuracy debuffs into account
		FVector AccuracyEnemyLocation = FVector(EnemyLocation.X * RandomAccuracy, EnemyLocation.Y * RandomAccuracy, EnemyLocation.Z);

		//Don't forget to take this guy into account!
		if (Enemy->IsA<ADDShrewdEnemy>()) {
			ADDShrewdEnemy* ShrewdEnemy = Cast<ADDShrewdEnemy>(Enemy);
			AccuracyEnemyLocation += ShrewdEnemy->GetRandomInaccuracy();
		}

		FVector EnemyToSentientVector = AccuracyEnemyLocation - GetActorLocation();

		float TimeToReachEnemy = 1;
		
		if (!FMath::IsNearlyZero(ProjectileSpeed)) {
			//You can find out how these equations were solved for in the math located at "Design/Math/predicting_enemy_movement.png"
			//
			//Quadratic equation calculations and error checking, solving for projectile time to reach enemy
			const float A = FVector::DotProduct(EnemyVelocity, EnemyVelocity) - FMathf::Pow(ProjectileSpeed, 2.0f);
			const float B = 2.0f * FVector::DotProduct(EnemyToSentientVector, EnemyVelocity);
			const float C = FVector::DotProduct(EnemyToSentientVector, EnemyToSentientVector);

			const float Discriminant = (B * B) - 4.0f * A * C;
			if (Discriminant < 0) {
				//Occurs if projectile is unable to reach the enemy
				UE_LOG(LogTemp, Warning, TEXT("Projectile unable to reach the enemy, destroying..."))
				Proj->ForceDestroyProjectile();
				return;
			}

			const float SqrtDiscriminant = FMathf::Sqrt(Discriminant); //A little time save to not do double sqrt
			const float TimeA = (-B + SqrtDiscriminant) / (2.0f * A);
			const float TimeB = (-B - SqrtDiscriminant) / (2.0f * A);

			if (TimeA < 0 && TimeB < 0) {
				//Occurs if projectile is unable to reach the enemy
				UE_LOG(LogTemp, Warning, TEXT("Projectile unable to reach the enemy, destroying..."))
				Proj->ForceDestroyProjectile();
				return;
			}
			else if (TimeA < 0) {
				TimeToReachEnemy = TimeB;
			}
			else if (TimeB < 0) {
				TimeToReachEnemy = TimeA;
			}
			else {
				//Fetch the fastest time to reach the enemy
				TimeToReachEnemy = FMathf::Min(TimeA, TimeB);
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Projectile speed should not be close to 0"))
		}
		
		//find out how this equations was solved for in the math located at "Design/Math/predicting_enemy_movement.png"
		FVector ProjDirection = (EnemyToSentientVector + EnemyVelocity * TimeToReachEnemy) / (ProjectileSpeed * TimeToReachEnemy);
		FVector PredictedEnemyLocation = AccuracyEnemyLocation + EnemyVelocity * TimeToReachEnemy;

		//ProjectileSpeed can be removed from the previous equation
		//but I leave it there just for learning sake, knowing how to get the direction only
		//removing it will just get me the velocity immediately
		ProjDirection.Z = 0.0f;

		Proj->SetVelocity(ProjDirection * ProjectileSpeed);
		Proj->SetDestination(PredictedEnemyLocation, ProjectileTimeTolerance);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s failed to spawn projectile!"), *GetName())
	}
}

void ADDSentientPlaceable::Attack(AActor* Actor, const bool bIsSpecificTarget, const float CustomDamage) const
{
	float OriginalAccuracy = 1.0f;
	ADDProjectile* Proj = GetWorld()->SpawnActor<ADDProjectile>(Projectile, GetActorLocation(), GetActorRotation());
	if (Proj) {

		Proj->SetProjectileOwner(this);
		Proj->SetCollisionChannelToIgnore(ECC_AttackRadiusChannel);
		Proj->SetCollisionChannelToIgnore(ECC_EnemyRadiusChannel);
		InitializeProjectile(Proj, CustomDamage);
		if (bIsSpecificTarget) {
			Proj->SetSpecificTarget(Cast<ALivingActor>(Actor));
		}
		else {
			Proj->SetCollisionChannelToIgnore(ECC_EnemyChannel);
		}

		FVector EnemyLocation = Actor->GetActorLocation();
		//Calculating inaccuracy, if any
		float RandomAccuracy = FMath::RandRange(OriginalAccuracy - Inaccuracy, OriginalAccuracy + Inaccuracy);
		FVector AccuracyEnemyLocation(EnemyLocation.X * RandomAccuracy, EnemyLocation.Y * RandomAccuracy, EnemyLocation.Z);

		FVector PlaceLocation = GetActorLocation();
		FVector ProjDirection = AccuracyEnemyLocation - PlaceLocation;
		ProjDirection.Normalize();

		Proj->SetVelocity(ProjDirection * ProjectileSpeed);
		Proj->SetDestination(EnemyLocation, ProjectileTimeTolerance);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s failed to spawn projectile!"), *GetName())
	}
}

void ADDSentientPlaceable::Deactive()
{
	//Have some things here that reset positioning of placeable
	//or whatever other things that need resetting
	StopAttack();
}

void ADDSentientPlaceable::EnableAI()
{
	bIsAIActive = true;
	StartAttack();
}

void ADDSentientPlaceable::DisableAI()
{
	bIsAIActive = false;
	StopAttack();
}

void ADDSentientPlaceable::SetAttackFunction(std::function<void()> Func)
{
	AttackFunc = Func;
}

void ADDSentientPlaceable::UpdateAttackRadius(const float NewAttackRadius)
{
	//This might or might not break things
	ActualAttackRadius += NewAttackRadius;
	AttackRadius = ActualAttackRadius;
	AttackCollider->SetSphereRadius(AttackRadius);
	AdjustAttackMesh();
}

void ADDSentientPlaceable::TriggerAttack()
{
	if (bIsAttacking && bIsAIActive) {
		AttackFunc();
	}
	else {
		GetWorldTimerManager().ClearTimer(AttackHandle);
	}
}

void ADDSentientPlaceable::InitializeState()
{
	UDDNormalSentientState* State = NewObject<UDDNormalSentientState>(this);
	if (State) {
		SetPlaceableState(State);
	}
	else {
		UE_LOG(LogTemp, Fatal, TEXT("Something went wrong with allocating Normal state"))
	}
}

void ADDSentientPlaceable::AdjustAttackMesh()
{
	FVector OriginalMeshSize = GetRadiusMeshSize();
	float CorrectedScale = AttackRadius * 2;
	//Scales in Y, Z to avoid making circle thicker
	FVector Scale(1, CorrectedScale / OriginalMeshSize.Y, CorrectedScale / OriginalMeshSize.Z);

	if (RadiusMesh) {
		RadiusMesh->SetRelativeScale3D(Scale);
	}
}