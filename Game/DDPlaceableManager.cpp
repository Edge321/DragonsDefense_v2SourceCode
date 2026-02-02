// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlaceableManager.h"
#include "Components/BillboardComponent.h"
#include "Kismet/GameplayStatics.h"
//My classes
#include "DragonsDefense_v2/Game/DDPlaceablePreview.h"
#include "DragonsDefense_v2/Game/DDPlaceablePurchaseInfo.h"
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/DDStressMeterTypes.h"
#include "DragonsDefense_v2/Characters/DDSentientPlaceable.h"
#include "DragonsDefense_v2/Characters/DDTrapPlaceable.h"
#include "DragonsDefense_v2/Characters/DDPaladinSentient.h"

// Sets default values
ADDPlaceableManager::ADDPlaceableManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ManagerIcon = CreateDefaultSubobject<UBillboardComponent>("ManagerIcon");

	RootComponent = ManagerIcon;
}

// Called when the game starts or when spawned
void ADDPlaceableManager::BeginPlay()
{
	Super::BeginPlay();

	Preview = Cast<ADDPlaceablePreview>(GetWorld()->SpawnActor(PreviewClass));
	
	if (Preview) {
		Preview->SetActorHiddenInGame(true);
		Preview->OnColliding.BindUObject(this, &ADDPlaceableManager::IsPreviewValid);
	}
	else {
		UE_LOG(LogTemp, Fatal, TEXT("Something went wrong with the preview!"))
	}

	CheckPreviewValidity();
	InitializePurchaseInfo();

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->OnGameOver.AddDynamic(this, &ADDPlaceableManager::GameOverEventFunction);
		GameMode->OnGameRestart.AddDynamic(this, &ADDPlaceableManager::GameOverEventFunction);
		GameMode->OnGameWon.AddDynamic(this, &ADDPlaceableManager::GameOverEventFunction);
		GameMode->OnWaveOver.AddDynamic(this, &ADDPlaceableManager::WaveOverEventFunction);
		GameMode->OnWaveStart.AddDynamic(this, &ADDPlaceableManager::WaveStartEventFunction);
		GameMode->OnPlacing.AddDynamic(this, &ADDPlaceableManager::SetPreviewStatus);
		GameMode->OnUpdateSouls.AddDynamic(this, &ADDPlaceableManager::IsPreviewValid);
	}

	//Needed for placeable info to fetch data properly
	for (TSubclassOf<ADDPlaceable> Place : PlaceablesToSpawn) {
		SpawnPlaceableOutsidePool(Place, FVector(10000, 10000, 0), FRotator::ZeroRotator);
	}
}

void ADDPlaceableManager::ChangePreviewMesh(UStaticMesh* Mesh, const FVector Scale)
{
	Preview->SetMesh(Mesh);
	Preview->SetScale(Scale);
	Preview->CalculateOffset();
}

bool ADDPlaceableManager::CheckPlaceableType(ADDPlaceable* Placeable)
{
	return Placeable->IsA<ADDSentientPlaceable>();
}

void ADDPlaceableManager::BroadcastEvilPlaceable(ADDPlaceable* Placeable)
{
	OnPlaceableTurnEvil.Broadcast(Placeable);
}

void ADDPlaceableManager::BroadcastDamagedPlaceable(ADDPlaceable* Placeable)
{
	OnPlaceableDamaged.Broadcast(Placeable);
}

void ADDPlaceableManager::BroadcastNormalPlaceable(ADDPlaceable* Placeable)
{
	OnPlaceableTurnNormal.Broadcast(Placeable);
}

void ADDPlaceableManager::OnPlaceableSold()
{
	PlaceablesSold++;
	TotalPlaceablesPlaced = FMathf::Clamp(TotalPlaceablesPlaced - 1, 0, INFINITY);	
}

void ADDPlaceableManager::SetPreviewStatus(bool IsPlacing) {
	Preview->SetActorHiddenInGame(!IsPlacing);
}

bool ADDPlaceableManager::IsPreviewDisabled() const
{
	return Preview->IsHidden();
}

FVector ADDPlaceableManager::GetPreviewLocation() const
{
	return Preview->GetActorLocation();
}

void ADDPlaceableManager::CanPlace(bool PlaceStatus)
{
	bCanPlace = PlaceStatus;
}

bool ADDPlaceableManager::IsBuyable() const
{
	return PlaceableInfo.IsBuyable();
}

void ADDPlaceableManager::ResetMods()
{
	PriceMod = 1;
	DamageMod = 1;
	AttackRadiusMod = 1;
	AttackSpeedMod = 1;
	InaccuracyMod = 0;
	ApplyNewMods();
}

void ADDPlaceableManager::SpawnPlaceable(TSubclassOf<ADDPlaceable> PlaceableClass, const FVector Location, const FRotator Rotation)
{
	 ADDPlaceable* Placeable = GetWorld()->SpawnActor<ADDPlaceable>(PlaceableClass, Location, Rotation);

	if (Placeable) {
		InitializeSpawnedPlaceable(Placeable);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Something went wrong with spawning a placeable!"))
	}
}

void ADDPlaceableManager::SetCurrentPlaceable(TSubclassOf<ADDPlaceable> PlaceableClass, int32 Price)
{
	//A bit of optimization
	if (CurrentPlaceableClass == PlaceableClass) {
		PlaceableInfo.SetPrice(Price); //Accounts for different buttons having the same placeabe but different price
		return;
	}

	CurrentPlaceableClass = PlaceableClass;

	//Not sure if this is the greatest way to get the scale of the actor
	ADDPlaceable* Placeable = GetWorld()->SpawnActor<ADDPlaceable>(PlaceableClass, GetActorLocation(), GetActorRotation());
	Placeable->SetActorEnableCollision(false);
	
	if (Placeable) {
		FVector Scale = Placeable->GetMesh()->GetComponentScale();
		UStaticMesh* Mesh = Placeable->GetMesh()->GetStaticMesh();
		ChangePreviewMesh(Mesh, Scale);
		PlaceableInfo.SetPrice(Price);

		//Sentient properties here
		if (CheckPlaceableType(Placeable)) {
			ADDSentientPlaceable* Sentient = Cast<ADDSentientPlaceable>(Placeable);
			float Radius = Sentient->GetAttackRadius();
			FVector Size = Sentient->GetRadiusMeshSize();
			//Only need Y for diameter
			Preview->SetRadiusSize(Radius, Size.Y);
			Preview->EnableAttackRadius();
		}
		else {
			Preview->DisableAttackRadius();
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Something went wrong with setting the preview object"))
	}

	Placeable->Destroy();
	IsPreviewValid();
}

const TArray<ADDPlaceable*> ADDPlaceableManager::GetPlaceablePool() const
{
	return PlaceablePool;
}

const int32 ADDPlaceableManager::GetTotalPlaceablesPlaced() const
{
	return TotalPlaceablesPlaced;
}

const TArray<TWeakObjectPtr<ADDSentientPlaceable>>& ADDPlaceableManager::GetSentientPool() const 
{
	return SentientReferences;
}

const TArray<TWeakObjectPtr<ADDTrapPlaceable>>& ADDPlaceableManager::GetTrapPool() const 
{
	return TrapReferences;
}

const int32 ADDPlaceableManager::GetBeginningPlaceableAmount() const
{
	return BeginningPlaceableAmount;
}

void ADDPlaceableManager::SetPriceMod(const float NewPriceMod)
{
	PriceMod = NewPriceMod;
}

void ADDPlaceableManager::SetDamageMod(const float NewDamageMod)
{
	DamageMod = NewDamageMod;
}

void ADDPlaceableManager::SetAttackRadiusMod(const float NewAttackRadiusMod)
{
	AttackRadiusMod = NewAttackRadiusMod;
}

void ADDPlaceableManager::SetAttackSpeedMod(const float NewAttackSpeedMod)
{
	AttackSpeedMod = NewAttackSpeedMod;
}

void ADDPlaceableManager::SetAccuracyMod(const float NewAccuracyMod)
{
	InaccuracyMod = NewAccuracyMod;
}

void ADDPlaceableManager::ApplyNewMods()
{
	for (TWeakObjectPtr<ADDSentientPlaceable> Sent : SentientReferences) {
		if (Sent.IsValid()) {
			ApplyMods(Sent.Get());
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Found invalid Sentient pointer. Removing..."))
			SentientReferences.Remove(Sent);
		}
	}

	for (TWeakObjectPtr<ADDTrapPlaceable> Trap : TrapReferences) {
		if (Trap.IsValid()) {
			ApplyMods(Trap.Get());
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Found invalid Trap pointer. Removing..."))
			TrapReferences.Remove(Trap);
		}
	}
}

void ADDPlaceableManager::ApplyMods(ADDSentientPlaceable* Placeable)
{
	Placeable->SetPriceMod(PriceMod);
	Placeable->SetDamageMod(DamageMod);
	Placeable->SetAttackRadiusMod(AttackRadiusMod);
	Placeable->SetAttackSpeedMod(AttackSpeedMod);
	Placeable->SetAccuracyMod(InaccuracyMod);
}

void ADDPlaceableManager::ApplyMods(ADDTrapPlaceable* Placeable)
{
	Placeable->SetPriceMod(PriceMod);
	Placeable->SetDamageMod(DamageMod);
}

void ADDPlaceableManager::ShowAllPlaceablesInfo()
{
	if (bIsShowingAllPlaceableInfo) return;
	if (PlaceablePool.Num() <= 0) return;

	for (int i = 0; i < PlaceablePool.Num(); i++) {
		PlaceablePool[i]->ShowHealthBar(true);
		//PlaceablePool[i]->EnableHighlight();
		if (i < SentientReferences.Num()) {
			SentientReferences[i]->ShowAttackRadius();
		}
		if (i < TrapReferences.Num()) {
			//TODO - later down the line extra info like health will be here
		}
	}

	bIsShowingAllPlaceableInfo = true;
	OnSetPlaceableInfoVisibility.ExecuteIfBound(true);
}

void ADDPlaceableManager::HideAllPlaceablesInfo()
{
	if (!bIsShowingAllPlaceableInfo) return;
	if (PlaceablePool.Num() <= 0) return;

	for (int i = 0; i < PlaceablePool.Num(); i++) {
		PlaceablePool[i]->HideHealthBar();
		//PlaceablePool[i]->DisableHighlight();
		if (i < SentientReferences.Num()) {
			SentientReferences[i]->HideAttackRadius();
		}
		if (i < TrapReferences.Num()) {
			//TODO - later down the line extra info like health will be here
		}
	}

	bIsShowingAllPlaceableInfo = false;
	OnSetPlaceableInfoVisibility.ExecuteIfBound(false);
}

void ADDPlaceableManager::SpawnPlaceableAtCursor(TSubclassOf<ADDPlaceable> PlaceableClass)
{
	ADDPlaceable* Placeable = GetWorld()->SpawnActor<ADDPlaceable>(PlaceableClass, GetPreviewLocation(), Preview->GetActorRotation());
	if (Placeable) {
		InitializeSpawnedPlaceable(Placeable);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Something went wrong with spawning a placeable at the cursor!"))
	}
}

void ADDPlaceableManager::PurchasePlaceableAtCursor()
{
	if (PlaceableInfo.IsBuyable() && bCanPlace) {
		PlaceableInfo.UpdateSouls();

		ADDPlaceable* Placeable = GetWorld()->SpawnActor<ADDPlaceable>(CurrentPlaceableClass, GetPreviewLocation(), Preview->GetActorRotation());
		if (Placeable) {
			InitializeSpawnedPlaceable(Placeable);
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Something went wrong with spawning a placeable at the cursor!"))
		}
	}
}

void ADDPlaceableManager::CheckPreviewValidity()
{
	check(Preview)
}

void ADDPlaceableManager::AddPlaceableToPool(ADDPlaceable* Placeable)
{
	TotalPlaceablesPlaced++;
	PlaceablePool.Add(Placeable);
	if (CheckPlaceableType(Placeable)) {
		ADDSentientPlaceable* Sentient = Cast<ADDSentientPlaceable>(Placeable);
		ApplyMods(Sentient);
		TWeakObjectPtr<ADDSentientPlaceable> SentientPtr = MakeWeakObjectPtr(Sentient);
		SentientReferences.Add(SentientPtr);
	}
	else {
		ADDTrapPlaceable* Trap = Cast<ADDTrapPlaceable>(Placeable);
		ApplyMods(Trap);
		TWeakObjectPtr<ADDTrapPlaceable> TrapPtr = MakeWeakObjectPtr(Trap);
		TrapReferences.Add(TrapPtr);
	}
}

void ADDPlaceableManager::RemovePlaceableFromPool(ADDPlaceable* Placeable)
{
	OnKilledPlaceable.Broadcast(Placeable);
	OnPlaceableDespawn.ExecuteIfBound(EDDStressMeterTypes::PlaceableDeath);

	ADDPlaceable* ToRemove = nullptr;

	for (ADDPlaceable* SomePlaceable : PlaceablePool) {
		if (SomePlaceable && SomePlaceable == Placeable) {
			ToRemove = SomePlaceable;
			break;
		}
	}

	if (ToRemove) {
		if (CheckPlaceableType(ToRemove)) {
			TWeakObjectPtr<ADDSentientPlaceable> SentientPtr = MakeWeakObjectPtr(Cast<ADDSentientPlaceable>(ToRemove));
			SentientReferences.RemoveSwap(SentientPtr);
		}
		else {
			TWeakObjectPtr<ADDTrapPlaceable> TrapPtr = MakeWeakObjectPtr(Cast<ADDTrapPlaceable>(ToRemove));
			TrapReferences.RemoveSwap(TrapPtr);
		}
		ToRemove->Destroy();
		PlaceablePool.RemoveSwap(ToRemove);

	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Placeable in pool was not found."))
	}
}

void ADDPlaceableManager::ClearPool()
{
	for (ADDPlaceable* Placeable : PlaceablePool) {
		if (Placeable) {
			Placeable->Destroy();
		}
	}
	PlaceablePool.Empty();
	TrapReferences.Empty();
	SentientReferences.Empty();
}

void ADDPlaceableManager::SpawnPlaceableOutsidePool(TSubclassOf<ADDPlaceable> PlaceableClass, const FVector Location, const FRotator Rotation)
{
	GetWorld()->SpawnActor<ADDPlaceable>(PlaceableClass, Location, Rotation);
}

void ADDPlaceableManager::InitializePurchaseInfo()
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());

	if (GameMode) {
		PlaceableInfo.SetGameMode(GameMode);
	}
	else {
		UE_LOG(LogTemp, Fatal, TEXT("How did we get here?"))
	}
}

void ADDPlaceableManager::InitializeSpawnedPlaceable(ADDPlaceable* Placeable)
{
	Placeable->SpawnDefaultController();
	Placeable->AutoPossessAI = EAutoPossessAI::Spawned;
	Placeable->OnPlaceableDeath.AddUObject(this, &ADDPlaceableManager::RemovePlaceableFromPool);
	Placeable->OnTurnEvil.BindUObject(this, &ADDPlaceableManager::BroadcastEvilPlaceable);
	Placeable->OnDamaged.BindUObject(this, &ADDPlaceableManager::BroadcastDamagedPlaceable);
	Placeable->OnTurnNormal.BindUObject(this, &ADDPlaceableManager::BroadcastNormalPlaceable);
	Placeable->OnSold.BindUObject(this, &ADDPlaceableManager::OnPlaceableSold);
	OnPlaceableSpawn.ExecuteIfBound(EDDStressMeterTypes::PlaceableSpawn);

	//This is if the player puts the placeable in the tinkerer's aura (like a fool)
	if (!Placeable->IsEvil()) {
		Placeable->SetSellingPrice(PlaceableInfo.GetPrice());
	}
	AddPlaceableToPool(Placeable);
}

void ADDPlaceableManager::GameOverEventFunction()
{
	ClearPool();
	ResetMods();
	TotalPlaceablesPlaced = 0;
	PlaceablesSold = 0;
}

void ADDPlaceableManager::WaveStartEventFunction()
{
	BeginningPlaceableAmount = PlaceablePool.Num();
	//Make it so placeables sold are voided from The King's calculations
	BeginningPlaceableAmount = FMathf::Clamp(BeginningPlaceableAmount - PlaceablesSold, 0, INFINITY);
	//Get paladins and evil sentients back to fighting after starting it up
	for (TWeakObjectPtr<ADDSentientPlaceable> Sentient : SentientReferences) {
		if (Sentient->IsEvil() || Sentient->IsA<ADDPaladinSentient>()) {
			Sentient->EnableAI();
		}
	}
}

void ADDPlaceableManager::WaveOverEventFunction()
{
	PlaceablesSold = 0;
	//Don't want evil sentients or paladins fighting post-wave
	for (TWeakObjectPtr<ADDSentientPlaceable> Sentient : SentientReferences) {
		if (Sentient->IsEvil() || Sentient->IsA<ADDPaladinSentient>()) {
			Sentient->DisableAI();
		}
	}
}
