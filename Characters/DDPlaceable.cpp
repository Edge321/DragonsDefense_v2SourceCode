// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlaceable.h"
#include "Components/WidgetComponent.h"
#include "Components/TimelineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/DDDifficulty.h"
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"
#include "DragonsDefense_v2/UI/DDEnemyHealthBar.h"

// Sets default values
ADDPlaceable::ADDPlaceable()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	FadeTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("FadeTimeline"));

	HealthBarWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Collider->SetCollisionObjectType(ECC_PlaceableChannel);
	DDColliderLibrary::SetCollisionChannelToIgnore(Collider, ECC_PlaceableChannel);
}

void ADDPlaceable::ApplyDifficultyModifiers() 
{
	Super::ApplyDifficultyModifiers();
}

void ADDPlaceable::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);

	if ((bHasBeenRecentlyAttacked && !bManuallyShowHealthBar) || bRecentlyPlaced) {
		TempHealthBarTime += DeltaTime;
		if (TempHealthBarTime > HealthBarFadeAwayDelay) {
			FadeTimeline->PlayFromStart();
			bHasBeenRecentlyAttacked = false;
			bRecentlyPlaced = false;
			TempHealthBarTime = 0.0f;
		}
	}
}

void ADDPlaceable::BeginPlay()
{
	Super::BeginPlay();

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->OnWaveStart.AddDynamic(this, &ADDPlaceable::WaveStartEventFunction);
		bIsPlacedPostWave = GameMode->GetPostWaveState();
	}

	ActualDamage = Damage;
	TempHealth = Health;

	InitializeHealthBar();
	InitializeFadingAnimationVariables();

	bRecentlyPlaced = true; //Health bar is visible on placement
}

void ADDPlaceable::OnConstruction(const FTransform& Transform)
{
	if (HealthClass) {
		HealthBarWidget->SetWidgetClass(HealthClass);
	}

	HealthBarWidget->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));

	if (PlaceableData) {
		PlaceableData->PlaceableName = PlaceableName;
		PlaceableData->PlaceableDescription = PlaceableDescription;
		PlaceableData->Damage = -Damage;
		PlaceableData->Health = Health;
	}
}

void ADDPlaceable::UpdateHealth(const float HealthModifier)
{
	TempHealth += HealthModifier;

#if !UE_GAME
	//Temporary line until health bars in-game exist
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%s's Health: %f"), *GetName(), TempHealth));
#endif

	if (TempHealth <= 0)
	{
		OnDeath();
	}
	bHasBeenRecentlyAttacked = true;
	ShowHealthBar();

	UpdateSellingPrice();
	OnDamaged.ExecuteIfBound(this);
	UpdateHealthBar();
}

void ADDPlaceable::SetPlaceableState(IDDPlaceableStateInterface* State)
{
	if (PlaceableState) {
		PlaceableState->OnUninitialize();
	}
	PlaceableState.SetObject(State->_getUObject());
	PlaceableState.SetInterface(State);
	PlaceableState->OnInitialize(this);
}

void ADDPlaceable::SetSellingPrice(const int32 Price)
{
	if (bIsPlacedPostWave) {
		SellingPrice = Price;
		ActualPrice = SellingPrice;
		TempPrice = ActualPrice;
	}
	else {
		ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GameMode) {
			EDifficulty Difficulty = GameMode->GetDifficulty();

			switch (Difficulty) {
				case EDifficulty::Easy:
					SellingPrice = FMath::Clamp(Price * EasySellingPriceMod, MinSellingPrice, Price);
					break;
				case EDifficulty::Normal:
					SellingPrice = FMath::Clamp(Price * NormalSellingPriceMod, MinSellingPrice, Price);
					break;
				case EDifficulty::Hard:
					SellingPrice = FMath::Clamp(Price * HardSellingPriceMod, MinSellingPrice, Price);
					break;
			}

			ActualPrice = SellingPrice;
			TempPrice = ActualPrice;
		}
	}
}

void ADDPlaceable::SetSellingPriceExactly(const int32 Price)
{
	SellingPrice = Price;
	ActualPrice = SellingPrice;
	TempPrice = ActualPrice;
	UpdateSellingPrice();
}

void ADDPlaceable::SetPriceMod(const float PriceMod)
{
	SellingPrice = ActualPrice * PriceMod;
	StoredPriceMod = PriceMod;
}

void ADDPlaceable::SetDamageMod(const float DamageMod)
{
	Damage = ActualDamage * DamageMod;
}

void ADDPlaceable::ShowHealthBar(const bool ConstantVisibility) 
{
	FadeTimeline->Stop();
	TempHealthBarTime = 0.0f;
	HealthBarWidget->SetTintColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)); //Health bar now fully visible
	bManuallyShowHealthBar = ConstantVisibility;
	bRecentlyPlaced = false;
}

void ADDPlaceable::HideHealthBar()
{
	if (bHasBeenRecentlyAttacked) return; //No need to do function, placeable's health bar is being shown from being attacked!
	
	FadeTimeline->PlayFromStart();
	bManuallyShowHealthBar = false;
	TempHealthBarTime = 0.0f;
}

void ADDPlaceable::InitializeHealthBar() 
{
	HealthBarWidget->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	HealthBarWidget->SetWorldRotation(FQuat(0.0f, 0.0f, 180.0f, 0.0f)); //Rotate towards the direction camera
	HealthBarWidget->SetCastShadow(false);
	
	HealthBarWidget->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));

	//Cache health widget pointer
	UDDEnemyHealthBar* HealthWidget = Cast<UDDEnemyHealthBar>(HealthBarWidget->GetWidget());
	if (HealthWidget) {
		HealthWidgetRef = MakeWeakObjectPtr(HealthWidget);
		HealthWidgetRef->SetHealthScale(Health);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Health bar widget class not set for %s!"), *GetName())
	}

	if (HealthBarWidget) {
		const FRotator HealthRotation = HealthBarWidget->GetComponentRotation();
		const FVector HealthLocation = HealthBarWidget->GetComponentLocation();

		const APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		const FVector CamLocation = CameraManager->GetCameraLocation();

		const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(HealthLocation, CamLocation);
		const FRotator NewRotation(FRotator(LookAtRotation.Pitch, HealthRotation.Yaw, HealthRotation.Roll));

		HealthBarWidget->SetWorldRotation(NewRotation);
	}
}

void ADDPlaceable::InitializeFadingAnimationVariables() 
{
	UCurveFloat* FadeCurve = NewObject<UCurveFloat>();
	FadeCurve->FloatCurve.AddKey(0.0f, 1.0f);
	FadeCurve->FloatCurve.AddKey(HealthBarFadeAwayTime, 0.0f);

	FOnTimelineFloat TimelineProgress;
	TimelineProgress.BindDynamic(this, &ADDPlaceable::FadeHealthBar);
	FadeTimeline->AddInterpFloat(FadeCurve, TimelineProgress);

	//FOnTimelineEvent TimelineFinished;
	//TimelineFinished.BindDynamic(this, &ADDPlaceable::OnHealthBarFaded);
	//FadeTimeline->SetTimelineFinishedFunc(TimelineFinished);
}

void ADDPlaceable::UpdateSellingPrice()
{
	ActualPrice = TempPrice * (TempHealth / Health);
	if (SellingPrice > 0) {
		SellingPrice = FMath::Clamp(ActualPrice * StoredPriceMod, MinSellingPrice, SellingPrice);
	}
}

void ADDPlaceable::UpdateHealthBar()
{
	if (HealthWidgetRef.IsValid()) {
		HealthWidgetRef->SetPercent(TempHealth / Health);
	}
}

void ADDPlaceable::FadeHealthBar(const float Value) 
{
	HealthBarWidget->SetTintColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, Value));
}

void ADDPlaceable::WaveStartEventFunction()
{
	if (bIsPlacedPostWave) {
		bIsPlacedPostWave = false;
		SetSellingPrice(SellingPrice);
	}
}

/*void ADDPlaceable::OnHealthBarFaded() 
{
	bHasBeenRecentlyAttacked = false;
	TempHealthBarTime = 0.0f;
}*/

const int32 ADDPlaceable::GetSellingPrice() const
{
	return SellingPrice;
}

const float ADDPlaceable::GetThreatLevel() const
{
	return ThreatLevel;
}

const FString ADDPlaceable::GetPlaceableName() const
{
	return PlaceableName;
}

const FString ADDPlaceable::GetDescription() const
{
	return PlaceableDescription;
}

const bool ADDPlaceable::IsEvil() const
{
	return bIsEvil;
}

void ADDPlaceable::Sell()
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->UpdateSouls(SellingPrice);
	}

	OnSold.ExecuteIfBound();
	OnPlaceableDeath.Broadcast(this);
}

void ADDPlaceable::OnDeath()
{
	OnPlaceableDeath.Broadcast(this);
}
