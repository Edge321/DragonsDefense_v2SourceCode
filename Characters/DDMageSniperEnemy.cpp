// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDMageSniperEnemy.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/DDPlaceableManager.h"
#include "DragonsDefense_v2/Characters/DDSentientPlaceable.h"

void ADDMageSniperEnemy::BeginPlay()
{
	Super::BeginPlay();

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameModeRef = MakeWeakObjectPtr(GameMode);
	}

	GetWorldTimerManager().SetTimer(TimeToSnipe, this, &ADDMageSniperEnemy::StartSniping, SnipeDelay, false);
	GetWorldTimerManager().SetTimer(TimeToBeDetected, this, &ADDMageSniperEnemy::EnableDetection, DetectionDelay, false);
}

void ADDMageSniperEnemy::Tick(float DeltaTime)
{
	if (bIsSniping) {
		//Stay in place
	}
	else {
		//Move forward soldier!
		Super::Tick(DeltaTime);
	}
}

void ADDMageSniperEnemy::SnipeSentient()
{
	const TArray<TWeakObjectPtr<ADDSentientPlaceable>>& Sentients = GameModeRef->GetPlaceableManager().GetSentientPool();

	if (Sentients.IsEmpty()) {
		StopSniping();
	}
	else {
		if (!SentientTarget.IsValid()) {
			SentientTarget = FindFurthestSentient(Sentients);
		}
		ShootPlaceable(SentientTarget);
	}
}

void ADDMageSniperEnemy::StartSniping()
{
	GetWorldTimerManager().SetTimer(ShootHandle, this, &ADDMageSniperEnemy::SnipeSentient, ShootCooldown, true);
	bIsSniping = true;
}

void ADDMageSniperEnemy::StopSniping()
{
	GetWorldTimerManager().ClearTimer(ShootHandle);
	bIsSniping = false;
}

void ADDMageSniperEnemy::EnableDetection()
{
	bIsSentientDetectable = true;
	OnTurnDetectable.Broadcast(this);
}

TWeakObjectPtr<ADDSentientPlaceable> ADDMageSniperEnemy::FindFurthestSentient(const TArray <TWeakObjectPtr<ADDSentientPlaceable>>& Sentients) const
{
	TWeakObjectPtr<ADDSentientPlaceable> ClosestSentient = Sentients[0];
	float MaxDistance = FVector::Dist(GetActorLocation(), ClosestSentient->GetActorLocation());

	for (const TWeakObjectPtr<ADDSentientPlaceable> Sentient : Sentients) {
		const float TempDistance = FVector::Dist(GetActorLocation(), Sentient->GetActorLocation());
		if (TempDistance > MaxDistance) {
			ClosestSentient = Sentient;
			MaxDistance = TempDistance;
		}
	}

	return ClosestSentient;
}
