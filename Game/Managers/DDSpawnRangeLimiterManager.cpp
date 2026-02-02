// Copyright (c) 2025, Edge Cope Corp. All rights reserved


#include "DDSpawnRangeLimiterManager.h"
#include "Components/BillboardComponent.h"
#include "Components/TimelineComponent.h"
//My classes
#include "DragonsDefense_v2/Game/DDDifficulty.h"
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/DDEnemySpawner.h"

// Sets default values
ADDSpawnRangeLimiterManager::ADDSpawnRangeLimiterManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpawnRangeIcon = CreateDefaultSubobject<UBillboardComponent>("Icon");
	FadeTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("FadeTimeline"));

	RootComponent = SpawnRangeIcon;
}

// Called when the game starts or when spawned
void ADDSpawnRangeLimiterManager::BeginPlay()
{
	Super::BeginPlay();

	CurrentSpawnRangeLimit = StartingSpawnRangeLimit;

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->OnGameStart.AddDynamic(this, &ADDSpawnRangeLimiterManager::GameStartEventFunction);
		GameMode->OnGameOver.AddDynamic(this, &ADDSpawnRangeLimiterManager::GameOverEventFunction);
		GameMode->OnGameRestart.AddDynamic(this, &ADDSpawnRangeLimiterManager::GameOverEventFunction);
		GameMode->OnGameWon.AddDynamic(this, &ADDSpawnRangeLimiterManager::GameOverEventFunction);
		GameMode->OnWaveStart.AddDynamic(this, &ADDSpawnRangeLimiterManager::WaveStartEventFunction);
		GameMode->OnWaveOver.AddDynamic(this, &ADDSpawnRangeLimiterManager::WaveOverEventFunction);
	}

	InitializeRedLineLimiters();
	InitializeSpawnPointers();
	InitializeFadingAnimationVariables();
}

// Called every frame
void ADDSpawnRangeLimiterManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bAllArrowsHidden) return;

	TotalTime += DeltaTime;

	const float Result = FMath::Sin(TotalTime * ArrowSpeed) * ArrowAmplitude; 

	for (int32 i = 0; i < SpawnPointers.Num(); i++) {
		const FVector Location = SpawnPointerLocations[i];
		SpawnPointers[i]->SetWorldLocation(FVector(Location.X + Result, Location.Y, Location.Z));
	}
}

void ADDSpawnRangeLimiterManager::SetTemporarySpawnRange(const FVector2D SpawnRange)
{
	TempSpawnRangeLimit = SpawnRange;
	bTempSpawnRangeLimitActive = true;
}

const FVector2D ADDSpawnRangeLimiterManager::GetCurrentSpawnRangeLimit() const 
{
	return CurrentSpawnRangeLimit;
}

float ADDSpawnRangeLimiterManager::LimitEnemySpawnRange(const float SpawnYAxis) const 
{
	return FMath::Clamp(SpawnYAxis, CurrentSpawnRangeLimit.X, CurrentSpawnRangeLimit.Y);
}

void ADDSpawnRangeLimiterManager::InitializeRedLineLimiters()
{
	//TODO - figure out math for doing a perpendicular red line for the left and right red line such that it covers the top of the screen
	const FVector Location = GetActorLocation();
	const FTransform ManagerTransform = GetActorTransform();
	FTransform LeftRedLineTransform = ManagerTransform;
	FTransform TopLeftRedLineTransform = ManagerTransform;
	FTransform RightRedLineTransform = ManagerTransform;
	FTransform TopRightRedLineTransform = ManagerTransform;
	LeftRedLineTransform.SetLocation(FVector(Location.X, StartingSpawnRangeLimit.X, Location.Z));
	TopLeftRedLineTransform.SetLocation(FVector(Location.X + TopRedLinesOffsetY, StartingSpawnRangeLimit.X - (TopRedLinesLength / 2.0f), Location.Z));
	TopLeftRedLineTransform.SetRotation(FRotator(0.0f, 90.0f, 0.0f).Quaternion());
	RightRedLineTransform.SetLocation(FVector(Location.X, StartingSpawnRangeLimit.Y, Location.Z));
	TopRightRedLineTransform.SetLocation(FVector(Location.X + TopRedLinesOffsetY, StartingSpawnRangeLimit.Y + (TopRedLinesLength / 2.0f), Location.Z));
	TopRightRedLineTransform.SetRotation(FRotator(0.0f, 90.0f, 0.0f).Quaternion());

	LeftRedLineLimiter = Cast<UStaticMeshComponent>(AddComponentByClass(RedLineLimiterClass, true, LeftRedLineTransform, false));
	TopLeftRedLineLimiter = Cast<UStaticMeshComponent>(AddComponentByClass(RedLineLimiterClass, true, TopLeftRedLineTransform, false));
	RightRedLineLimiter = Cast<UStaticMeshComponent>(AddComponentByClass(RedLineLimiterClass, true, RightRedLineTransform, false));
	TopRightRedLineLimiter = Cast<UStaticMeshComponent>(AddComponentByClass(RedLineLimiterClass, true, TopRightRedLineTransform, false));

	LeftRedLineLimiter->RegisterComponent();
	TopLeftRedLineLimiter->RegisterComponent();
	RightRedLineLimiter->RegisterComponent();
	TopRightRedLineLimiter->RegisterComponent();
	LeftRedLineLimiter->AttachToComponent(SpawnRangeIcon, FAttachmentTransformRules::KeepWorldTransform);
	TopLeftRedLineLimiter->AttachToComponent(SpawnRangeIcon, FAttachmentTransformRules::KeepWorldTransform);
	RightRedLineLimiter->AttachToComponent(SpawnRangeIcon, FAttachmentTransformRules::KeepWorldTransform);
	TopRightRedLineLimiter->AttachToComponent(SpawnRangeIcon, FAttachmentTransformRules::KeepWorldTransform);

	const FVector MeshSize = LeftRedLineLimiter->GetStaticMesh()->GetBoundingBox().GetSize();
	const FVector Scale = FVector(RedLineLength / MeshSize.X, 0.5f, 0.5f);
	const FVector TopScale = FVector(TopRedLinesLength / MeshSize.X, 0.5f, 0.5f);
	LeftRedLineLimiter->SetWorldScale3D(Scale);
	TopLeftRedLineLimiter->SetWorldScale3D(TopScale);
	RightRedLineLimiter->SetWorldScale3D(Scale);
	TopRightRedLineLimiter->SetWorldScale3D(TopScale);

	LeftRedLineLimiter->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TopLeftRedLineLimiter->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightRedLineLimiter->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TopRightRedLineLimiter->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADDSpawnRangeLimiterManager::InitializeSpawnPointers()
{
	//*2 is including both the left and right red line
	int32 ArrowAmount = (((FMath::Abs(SpawnRangeMax.X) + SpawnRangeMax.Y) - (ArrowSpacingBetweenRedLines * 2.0f)) / ArrowSpacing);
	//+1 is to include the middle most arrow, not factored in otherwise when it comes to even amount. Located at 0 on the Y-axis
	if (ArrowAmount % 2 == 0) {
		ArrowAmount++;
	}
	const FTransform ManagerTransform = GetActorTransform();
	const FVector ManagerLocation = ManagerTransform.GetLocation();
	FTransform NewTransform = ManagerTransform;
	NewTransform.SetLocation(FVector(ManagerLocation.X + ArrowXOffset, ManagerLocation.Y, ManagerLocation.Z + ArrowZOffset));

	const int32 MiddleArrow = ArrowAmount / 2;
	float CurrentYAxisLocation = (MiddleArrow * ArrowSpacing) * -1.0f; //Start from the negative side, and move all the way to the right side

	//Creating two arrows, one on the negative Y axis, one on the positive Y axis
	for (int32 i = 0; i < ArrowAmount; i++)
	{
		TArray<TWeakObjectPtr<UMaterialInstanceDynamic>> CurrentSpawnPointerMaterials;
		NewTransform.SetLocation(FVector(NewTransform.GetLocation().X, CurrentYAxisLocation, NewTransform.GetLocation().Z));
		SpawnPointerLocations.Add(NewTransform.GetLocation());

		UStaticMeshComponent* SpawnPointer = Cast<UStaticMeshComponent>(AddComponentByClass(SpawnPointerClass, true, NewTransform, false));
		UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(SpawnPointer->GetMaterial(0), SpawnPointer);
		UMaterialInstanceDynamic* DynamicMaterialTwo = UMaterialInstanceDynamic::Create(SpawnPointer->GetMaterial(1), SpawnPointer);

		SpawnPointer->SetMaterial(0, DynamicMaterial);
		SpawnPointer->SetMaterial(1, DynamicMaterialTwo);

		SpawnPointer->SetRelativeScale3D(ArrowScale);
		SpawnPointer->SetVisibility(CurrentYAxisLocation > (StartingSpawnRangeLimit.X + ArrowSpacingBetweenRedLines) && 
									CurrentYAxisLocation < (StartingSpawnRangeLimit.Y - ArrowSpacingBetweenRedLines));

		CurrentSpawnPointerMaterials.Add(MakeWeakObjectPtr(DynamicMaterial));
		CurrentSpawnPointerMaterials.Add(MakeWeakObjectPtr(DynamicMaterialTwo));
		SpawnPointerDynamicMaterials.Add(CurrentSpawnPointerMaterials);
		SpawnPointers.Add(SpawnPointer);

		CurrentYAxisLocation += ArrowSpacing;
	}
}

void ADDSpawnRangeLimiterManager::InitializeFadingAnimationVariables() 
{
	UCurveFloat* FadeCurve = NewObject<UCurveFloat>();
	FadeCurve->FloatCurve.AddKey(0.0f, 1.0f);
	FadeCurve->FloatCurve.AddKey(FadingTime, 0.0f);

	FOnTimelineFloat TimelineProgress;
	TimelineProgress.BindDynamic(this, &ADDSpawnRangeLimiterManager::FadeSpawnPointers);
	FadeTimeline->AddInterpFloat(FadeCurve, TimelineProgress);

	FOnTimelineEvent TimelineFinished;
	TimelineFinished.BindDynamic(this, &ADDSpawnRangeLimiterManager::OnSpawnPointersInvisible);
	FadeTimeline->SetTimelineFinishedFunc(TimelineFinished);
}

void ADDSpawnRangeLimiterManager::ShowSpawnPointersWithinLimit() 
{
	//Setting visibility of arrows properly
	const int32 MidIndex = (SpawnPointers.Num() - 1) / 2;
	SpawnPointers[MidIndex]->SetVisibility(true); //Middle most arrow is always true

	//Function looking kinda ugly with these for loops
	for (TWeakObjectPtr<UMaterialInstanceDynamic> Dynamic : SpawnPointerDynamicMaterials[MidIndex]) {
		Dynamic->SetScalarParameterValue("Opacity", 1.0f); //Fully visible
	}

	for (int32 i = 1; i <= MidIndex; i++) {
		for (TWeakObjectPtr<UMaterialInstanceDynamic> Dynamic : SpawnPointerDynamicMaterials[MidIndex - 1]) {
			Dynamic->SetScalarParameterValue("Opacity", 1.0f); //Fully visible
		}
		for (TWeakObjectPtr<UMaterialInstanceDynamic> Dynamic : SpawnPointerDynamicMaterials[MidIndex + 1]) {
			Dynamic->SetScalarParameterValue("Opacity", 1.0f); //Fully visible
		}

		UStaticMeshComponent* LeftSpawnPointer = SpawnPointers[MidIndex - 1];
		UStaticMeshComponent* RightSpawnPointer = SpawnPointers[MidIndex + 1];
		LeftSpawnPointer->SetVisibility(LeftSpawnPointer->GetComponentLocation().Y > (CurrentSpawnRangeLimit.X + ArrowSpacingBetweenRedLines));
		RightSpawnPointer->SetVisibility(RightSpawnPointer->GetComponentLocation().Y < (CurrentSpawnRangeLimit.Y - ArrowSpacingBetweenRedLines));
	}

	bAllArrowsHidden = false;
}

void ADDSpawnRangeLimiterManager::HideAllSpawnPointersInstantly() 
{
	for (UStaticMeshComponent* SpawnPointer : SpawnPointers) { 
		SpawnPointer->SetVisibility(false);
	}
	OnSpawnPointersInvisible();
}

void ADDSpawnRangeLimiterManager::HideAllSpawnPointersFading() 
{
	GetWorldTimerManager().SetTimer(FadeTimerHandle, 
		[this]() { //lambda function used cus WorldTimerManager doesn't like external functions
			FadeTimeline->PlayFromStart();
		}, 
		FadingDelay, 
		false, 
		FadingDelay);
}

void ADDSpawnRangeLimiterManager::FadeSpawnPointers(const float Value) 
{
	for (int32 i = 0; i < SpawnPointers.Num(); i++) {
		if (SpawnPointers[i]->IsVisible()) {
			for (TWeakObjectPtr<UMaterialInstanceDynamic> Dynamic : SpawnPointerDynamicMaterials[i]) {
				Dynamic->SetScalarParameterValue("Opacity", Value);
			}
		}
	}
}

void ADDSpawnRangeLimiterManager::GameStartEventFunction() 
{
	if (!EnemySpawnerRef.IsValid()) {
		EnemySpawnerRef = &(Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode())->GetEnemySpawner());
	}

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		FVector2D TempStartSpawnRange = StartingSpawnRangeLimit;
		const FVector RedLineLocation = LeftRedLineLimiter->GetComponentLocation();
		const EDifficulty Difficulty = GameMode->GetDifficulty();

		switch (Difficulty) {
			case EDifficulty::Easy:
				TempStartSpawnRange *= EasySpawnRangeLimitModifier;
				break;
			case EDifficulty::Normal:
			default:
				//Nothing happens
				break;
			case EDifficulty::Hard:
				TempStartSpawnRange *= HardSpawnRangeLimitModifier;
				break;
		}

		LeftRedLineLimiter->SetWorldLocation(FVector(RedLineLocation.X, TempStartSpawnRange.X, RedLineLocation.Z));
		RightRedLineLimiter->SetWorldLocation(FVector(RedLineLocation.X, TempStartSpawnRange.Y, RedLineLocation.Z));
		
		const FVector TopLeftRedLocation = FVector(TopLeftRedLineLimiter->GetComponentLocation().X, TempStartSpawnRange.X - (TopRedLinesLength / 2.0f), RedLineLocation.Z);
		const FVector TopRightRedLocation = FVector(TopRightRedLineLimiter->GetComponentLocation().X, TempStartSpawnRange.Y + (TopRedLinesLength / 2.0f), RedLineLocation.Z);

		TopLeftRedLineLimiter->SetWorldLocation(TopLeftRedLocation);
		TopRightRedLineLimiter->SetWorldLocation(TopRightRedLocation);

		CurrentSpawnRangeLimit = TempStartSpawnRange;
	}

	HideAllSpawnPointersFading();
}

void ADDSpawnRangeLimiterManager::GameOverEventFunction() 
{
	CurrentSpawnRangeLimit = StartingSpawnRangeLimit;

	const FVector LeftRedLineLocation = LeftRedLineLimiter->GetComponentLocation();
	const FVector RightRedLineLocation = RightRedLineLimiter->GetComponentLocation();
	LeftRedLineLimiter->SetWorldLocation(FVector(LeftRedLineLocation.X, CurrentSpawnRangeLimit.X, LeftRedLineLocation.Z));
	RightRedLineLimiter->SetWorldLocation(FVector(RightRedLineLocation.X, CurrentSpawnRangeLimit.Y, RightRedLineLocation.Z));

	const FVector TopLeftRedLocation = FVector(TopLeftRedLineLimiter->GetComponentLocation().X, CurrentSpawnRangeLimit.X - (TopRedLinesLength / 2.0f), LeftRedLineLocation.Z);
	const FVector TopRightRedLocation = FVector(TopRightRedLineLimiter->GetComponentLocation().X, CurrentSpawnRangeLimit.Y + (TopRedLinesLength / 2.0f), RightRedLineLocation.Z);
	TopLeftRedLineLimiter->SetWorldLocation(TopLeftRedLocation);
	TopRightRedLineLimiter->SetWorldLocation(TopRightRedLocation);

	ShowSpawnPointersWithinLimit();
}

void ADDSpawnRangeLimiterManager::WaveStartEventFunction()
{
	HideAllSpawnPointersFading();
}

void ADDSpawnRangeLimiterManager::WaveOverEventFunction()
{
	if (EnemySpawnerRef->GetCurrentWave() % SpawnRangeIncreaseWave == 0)
	{
		const FVector RedLineLocation = LeftRedLineLimiter->GetComponentLocation();

		CurrentSpawnRangeLimit.X -= SpawnRangeIncrease;
		CurrentSpawnRangeLimit.Y += SpawnRangeIncrease;

		LeftRedLineLimiter->SetWorldLocation(FVector(RedLineLocation.X, CurrentSpawnRangeLimit.X, RedLineLocation.Z));
		RightRedLineLimiter->SetWorldLocation(FVector(RedLineLocation.X, CurrentSpawnRangeLimit.Y, RedLineLocation.Z));

		const FVector TopLeftRedLocation = FVector(TopLeftRedLineLimiter->GetComponentLocation().X, CurrentSpawnRangeLimit.X - (TopRedLinesLength / 2.0f), RedLineLocation.Z);
		const FVector TopRightRedLocation = FVector(TopRightRedLineLimiter->GetComponentLocation().X, CurrentSpawnRangeLimit.Y + (TopRedLinesLength / 2.0f), RedLineLocation.Z);
		TopLeftRedLineLimiter->SetWorldLocation(TopLeftRedLocation);
		TopRightRedLineLimiter->SetWorldLocation(TopRightRedLocation);
	}

	ShowSpawnPointersWithinLimit();
}

void ADDSpawnRangeLimiterManager::OnSpawnPointersInvisible() 
{
	bAllArrowsHidden = true;
}
