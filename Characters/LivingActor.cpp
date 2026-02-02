// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "LivingActor.h"
//My classes
#include "DragonsDefense_v2/Game/Debuffs/DDTickBasedDebuffBase.h"
#include "DragonsDefense_v2/Game/Debuffs/DDTimeBasedDebuffBase.h"

// Sets default values
ALivingActor::ALivingActor()
{
	//Class that inherits is responsible for setting the root component
	Collider = CreateDefaultSubobject<UBoxComponent>("BoxCollider");
	Arrow = CreateDefaultSubobject<UArrowComponent>("ActorDirection");
}

void ALivingActor::BeginPlay()
{
	Super::BeginPlay();

	FVector Stats = FVector(SecretStats.Offense,
		SecretStats.Defense,
		SecretStats.Support);

	//Damn this is some ugly ass code
	//X is offense, Y is defense, Z is support
	if (Stats.X > Stats.Y && Stats.X > Stats.Z)
	{
		LivingActorStyle = EDDLivingActorStyle::Aggressive;
	}
	else if (Stats.Y > Stats.X && Stats.Y > Stats.Z)
	{
		LivingActorStyle = EDDLivingActorStyle::Defensive;
	}
	else if (Stats.Z > Stats.X && Stats.Z > Stats.Y)
	{
		LivingActorStyle = EDDLivingActorStyle::Supportive;
	}
	else
	{
		LivingActorStyle = EDDLivingActorStyle::Generalist;
	}
}

// Called every frame
void ALivingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateDebuffs(DeltaTime);
}

const float ALivingActor::GetHealth() const
{
	return Health;
}

void ALivingActor::UpdateHealth(const float HealthModifier)
{
	Health += HealthModifier;

#if !UE_GAME
	//Temporary line until health bars in-game exist
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%s's Health: %f"), *GetName(), Health));
#endif

	if (Health <= 0)
	{
		OnDeath();
	}
}

const float ALivingActor::GetDamage() const
{
	return Damage;
}

const EDDLivingActorStyle ALivingActor::GetLivingActorStyle() const
{
	return LivingActorStyle;
}

const FDDSecretStats ALivingActor::GetSecretStats() const
{
	return SecretStats;
}

FVector ALivingActor::GetColliderSizeWorldSpace() const
{
	return Collider->GetUnscaledBoxExtent() * Collider->GetRelativeScale3D();
}

void ALivingActor::SetDamage(const float NewDamage)
{
	Damage = NewDamage;
}

void ALivingActor::UpdateDamage(const float NewDamage)
{
	Damage += NewDamage;
}

void ALivingActor::UpdateSecretStats(const FDDSecretStats NewSecretStats)
{
	//Update secret stats only once, and only once!!!!
	if (bHasUpdatedSecretStats) return;
	bHasUpdatedSecretStats = true;
	SecretStats += NewSecretStats;
}

void ALivingActor::AddDebuff(UDDDebuffBase* Debuff)
{
	if (Debuff) {
		Debuff->ApplyDebuff(this);
		ActiveDebuffs.Add(Debuff);
		Debuff->OnRemoveDebuff.BindUObject(this, &ALivingActor::RemoveDebuff);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error: Invalid Debuff passed in"))
	}
}

void ALivingActor::AddDebuff(UDDTickBasedDebuffBase* Debuff)
{
	if (Debuff) {
		AddDebuff(Cast<UDDDebuffBase>(Debuff));
		TickDebuffRefs.Add(MakeWeakObjectPtr(Debuff));
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error: Invalid Debuff passed in"))
	}
}

void ALivingActor::AddDebuff(UDDTimeBasedDebuffBase* Debuff)
{
	if (Debuff) {
		AddDebuff(Cast<UDDDebuffBase>(Debuff));
		Debuff->StartDebuff();
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error: Invalid Debuff passed in"))
	}
}

void ALivingActor::RemoveDebuff(UDDDebuffBase* Debuff)
{
	if (Debuff->IsA<UDDTickBasedDebuffBase>()) {
		UDDTickBasedDebuffBase* TickDebuff = Cast<UDDTickBasedDebuffBase>(Debuff);
		TickDebuffRefs.RemoveSwap(TickDebuff);
	}
	ActiveDebuffs.RemoveSwap(Debuff);
}

void ALivingActor::UpdateDebuffs(float DeltaTime)
{
	//Use of lambda function to find elements that are invalid to be removed
	//Efficient way as it is one pass through
	TickDebuffRefs.RemoveAllSwap([&](const TWeakObjectPtr<UDDTickBasedDebuffBase>& TickDebuff) {
		if (TickDebuff.IsValid()) {
			TickDebuff->UpdateDebuff(DeltaTime);
			return false;
		}
		else {
			return true;
		}
	});
}