// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
//My classes
#include "DragonsDefense_v2/Characters/DDPlayer.h"
#include "DragonsDefense_v2/Game/DDSaveGame.h"
#include "DragonsDefense_v2/Game/DDProjectileManager.h"
#include "DragonsDefense_v2/Game/DDScoreWidget.h"
#include "DragonsDefense_v2/Game/DDDifficulty.h"
#include "DragonsDefense_v2/Game/DDEnemySpawner.h"
#include "DragonsDefense_v2/Game/DDNegativeSoulShopManager.h"
#include "DragonsDefense_v2/Game/DDClickableManager.h"
#include "DragonsDefense_v2/Game/DDPlaceableManager.h"
#include "DragonsDefense_v2/Game/Managers/DDSpawnRangeLimiterManager.h"
#include "DragonsDefense_v2/Game/Managers/DDDebuffAreaManager.h"
#include "DragonsDefense_v2/Game/Managers/DDCraterManager.h"
#include "DragonsDefense_v2/Game/Managers/DDTheKing.h"
#include "DragonsDefense_v2/UI/DDMainMenuWidget.h"
#include "DragonsDefense_v2/UI/DDSoulShopWidget.h"
#include "DragonsDefense_v2/UI/DDHealthBarWidget.h"
#include "DragonsDefense_v2/UI/DDNegativeSoulShopWidget.h"
#include "DragonsDefense_v2/UI/Villagerpedia/DDVillagerpediaWidget.h"
#include "DragonsDefense_v2/UI/DDPauseMenuWidget.h"
#include "DragonsDefense_v2/UI/DDNewVillagerAlertWidget.h"
#include "DragonsDefense_v2/UI/DDKingAnnouncementsWidget.h"

void ADDGameModeBase::BeginPlay()
{
	SoulShopWidget = AddWidgetToViewport<UDDSoulShopWidget>(SoulShopWidgetClass, 1);
	NegativeShopWidget = AddWidgetToViewport<UDDNegativeSoulShopWidget>(NegativeShopWidgetClass, 0);
	MainMenuWidget = AddWidgetToViewport<UDDMainMenuWidget>(MainMenuWidgetClass);
	HealthBarWidget = AddWidgetToViewport<UDDHealthBarWidget>(HealthBarWidgetClass, -1);
	ScoreWidget = AddWidgetToViewport<UDDScoreWidget>(ScoreWidgetClass, -2);
	VillagerpediaWidget = AddWidgetToViewport<UDDVillagerpediaWidget>(VillagerpediaWidgetClass, 5);
	PauseMenuWidget = AddWidgetToViewport<UDDPauseMenuWidget>(PauseMenuWidgetClass, 10);
	NewVillagerAlertWidget = AddWidgetToViewport<UDDNewVillagerAlertWidget>(NewVillagerAlertWidgetClass, 3);
	KingAnnouncementsWidget = AddWidgetToViewport<UDDKingAnnouncementsWidget>(KingAnnouncementsWidgetClass, 4);

	//TODO - Maybe find them all at the same time in one run instead of multiple iterations?
	ProjectileManager = FindUObject<ADDProjectileManager>();
	PlaceableManager = FindUObject<ADDPlaceableManager>();
	NegativeShopManager = FindUObject<ADDNegativeSoulShopManager>();
	Player = FindUObject<ADDPlayer>();
	EnemySpawner = FindUObject<ADDEnemySpawner>();
	SpawnRangeLimiterManager = FindUObject<ADDSpawnRangeLimiterManager>();
	ClickableManager = FindUObject<ADDClickableManager>();
	CraterManager = FindUObject<ADDCraterManager>();
	DebuffAreaManager = FindUObject<ADDDebuffAreaManager>();
	TheKing = FindUObject<ADDTheKing>();
	Difficulty = EDifficulty::Normal;

	NegativeShopWidget->SetManager(NegativeShopManager);
	NegativeShopManager->SetNegativeShop(NegativeShopWidget);
	MainMenuWidget->OnMainMenuVillagerpedia.BindUObject(this, &ADDGameModeBase::ExecuteOpenVillagerpediaDelegate);
	PauseMenuWidget->OnPauseMenuVillagerpedia.BindUObject(this, &ADDGameModeBase::ExecuteOpenVillagerpediaDelegate);
	MainMenuWidget->OnToggleEnemyAttackBoxes.BindUObject(EnemySpawner, &ADDEnemySpawner::ToggleEnemyAttackBoxesEventFunction);

	if (bFullSaveDebug) {
		TArray<int> IDArray;
		for (int32 i = 0; i < (int32) EDDEnemyIDs::ENEMY_TOTAL; i++) IDArray.Add(i);
		VillagerpediaWidget->LoadFoundVillagers(IDArray);
		SoulShopWidget->LoadUnlockedButtons(999999999);
	}
	else {
		UDDSaveGame* LoadData = Cast<UDDSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
		if (LoadData) {
			VillagerpediaWidget->LoadFoundVillagers(LoadData->VillagersDiscovered);
			SoulShopWidget->LoadUnlockedButtons(LoadData->WaveHighScore);
			//Difficulties won is loaded in the InitGame() function
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("No save found, skipping..."))
		}
	}
}

void ADDGameModeBase::UpdateScoreText()
{
	ScoreWidget->SetScoreText(TotalEnemiesKilled);
}

void ADDGameModeBase::UpdateFinalScoreText() 
{
	ScoreWidget->SetFinalScoreText(TotalEnemiesKilled);
}

void ADDGameModeBase::UpdateSoulsText()
{
	SoulShopWidget->UpdateSoulsText(TotalSouls);
}

void ADDGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	for (int32 i = 0; i < (int32)EDifficulty::DIFFICULTIES_TOTAL; i++) {
		DifficultiesWon.Add((EDifficulty)i, false);
	}

	if (bFullSaveDebug) {
		for (int32 i = 0; i < (int32)EDifficulty::DIFFICULTIES_TOTAL; i++) {
			DifficultiesWon[(EDifficulty)i] = true;
		}
	}
	else {
		UDDSaveGame* LoadData = Cast<UDDSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
		if (LoadData) {
			for (int32 DifficultyInt : LoadData->DifficultiesWon) {
				DifficultiesWon[(EDifficulty)DifficultyInt] = true;
			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("No save found, skipping..."))
		}
	}
}

void ADDGameModeBase::AddScore(int32 Score)
{
	TotalEnemiesKilled += Score;
	UpdateScoreText();
}

void ADDGameModeBase::UpdateSouls(int32 Souls)
{
	TotalSouls += Souls;
	//Negative means something was bought, we want to keep accumulation tho!!!!
	if (Souls > 0) { 
		TotalAcummulatedSouls += Souls;
	}
	UpdateSoulsText();
	OnUpdateSouls.Broadcast();
}

ADDProjectileManager& ADDGameModeBase::GetProjectileManager()
{
	//check(ProjectileManager)
	return *ProjectileManager;
}

ADDPlaceableManager& ADDGameModeBase::GetPlaceableManager()
{
	//check(PlaceableManager)
	return *PlaceableManager;
}

ADDEnemySpawner& ADDGameModeBase::GetEnemySpawner()
{
	//check(EnemySpawner)
	return *EnemySpawner;
}

ADDSpawnRangeLimiterManager& ADDGameModeBase::GetSpawnRangeLimiterManager() 
{
	return *SpawnRangeLimiterManager;
}

ADDPlayer& ADDGameModeBase::GetPlayer()
{
	//check(Player)
	return *Player;
}

ADDClickableManager& ADDGameModeBase::GetClickableManager()
{
	//check(ClickableManager)
	return *ClickableManager;
}

ADDCraterManager& ADDGameModeBase::GetCraterManager()
{
	//check(CraterManager)
	return *CraterManager;
}

ADDDebuffAreaManager& ADDGameModeBase::GetDebuffAreaManager()
{
	//check(DebuffAreaManager)
	return *DebuffAreaManager;
}

UDDNegativeSoulShopWidget& ADDGameModeBase::GetNegativeSoulShopWidget()
{
	//check(NegativeShopWidget)
	return *NegativeShopWidget;
}

ADDNegativeSoulShopManager& ADDGameModeBase::GetNegativeSoulShopManager()
{
	return *NegativeShopManager;
}

UDDNewVillagerAlertWidget& ADDGameModeBase::GetNewVillagerAlertWidget()
{
	return *NewVillagerAlertWidget;
}

UDDSoulShopWidget& ADDGameModeBase::GetSoulShopWidget()
{
	return *SoulShopWidget;
}

ADDTheKing& ADDGameModeBase::GetTheKing()
{
	return *TheKing;
}

const EDifficulty ADDGameModeBase::GetDifficulty() const
{
	return Difficulty;
}

void ADDGameModeBase::SetDifficulty(const EDifficulty NewDifficulty)
{
	Difficulty = NewDifficulty;
}

const int32 ADDGameModeBase::GetSouls() const
{
	return TotalSouls;
}

const int32 ADDGameModeBase::GetAcummulatedSouls() const
{
	return TotalAcummulatedSouls;
}

const int32 ADDGameModeBase::GetBeginningSouls() const
{
	return BeginningSoulAmount;
}

UDDMainMenuWidget* ADDGameModeBase::GetMainMenuWidget() const
{
	check(MainMenuWidget)
	return MainMenuWidget;
}

const bool ADDGameModeBase::GetPauseState() const
{
	return bIsPaused;
}

const bool ADDGameModeBase::GetPostWaveState() const
{
	return bIsPostWave;
}

const TArray<EDifficulty> ADDGameModeBase::GetDifficultiesWon() const
{
	TArray<EDifficulty> DifficultiesWonArray;
	for (TPair<EDifficulty, bool> DifficultyPair : DifficultiesWon) {
		if (DifficultyPair.Value) {
			DifficultiesWonArray.Add(DifficultyPair.Key);
		}
	}
	return DifficultiesWonArray;
}

const int32 ADDGameModeBase::GetTheKingBossWave() const
{
	return TheKingBossWave;
}

void ADDGameModeBase::SetPauseState(const bool PauseState)
{
	bIsPaused = PauseState;
	OnPauseChangeState.Broadcast(bIsPaused);
}

void ADDGameModeBase::GameWon()
{
	DifficultiesWon[Difficulty] = true;
	SaveGame();

	OnGameWon.Broadcast();
	ResetGameData();
}

void ADDGameModeBase::GameOver()
{
	SaveGame();

	OnGameOver.Broadcast();
	ResetGameData();
}

void ADDGameModeBase::GameStart() const
{
	OnGameStart.Broadcast();
}

void ADDGameModeBase::GameRestart()
{
	OnGameRestart.Broadcast();
	ResetGameData();
}

void ADDGameModeBase::WaveOver()
{
	bIsPostWave = true;
	EnemySpawner->SetCurrentWave(EnemySpawner->GetCurrentWave() + 1);
	OnWaveOver.Broadcast();
	OnPlacing.Broadcast(false);
	WaveHighScore = EnemySpawner->GetCurrentWave() > WaveHighScore ? EnemySpawner->GetCurrentWave() : WaveHighScore;

	SaveGame();
}

void ADDGameModeBase::WaveStart()
{
	bIsPostWave = false;
	BeginningSoulAmount = TotalSouls;
	OnWaveStart.Broadcast();
}

void ADDGameModeBase::IsPlacing(bool Placing) const
{
	OnPlacing.Broadcast(Placing);
}

void ADDGameModeBase::ExecuteOpenVillagerpediaDelegate(UUserWidget* Widget)
{
	OnOpenVillagerpedia.Broadcast(Widget);
}

ADDPlayer* ADDGameModeBase::BlueprintGetPlayer() const
{
	return Player;
}

ADDProjectileManager* ADDGameModeBase::BlueprintGetProjectileManager() const
{
	return ProjectileManager;
}

ADDPlaceableManager* ADDGameModeBase::BlueprintGetPlaceableManager() const
{
	return PlaceableManager;
}

ADDNegativeSoulShopManager* ADDGameModeBase::BlueprintGetNegativeSoulShopManager()
{
	return NegativeShopManager;
}

UDDNegativeSoulShopWidget* ADDGameModeBase::BlueprintGetNegativeSoulShopWidget() const
{
	return NegativeShopWidget;
}

ADDEnemySpawner* ADDGameModeBase::BlueprintGetEnemySpawner() const
{
	return EnemySpawner;
}

ADDSpawnRangeLimiterManager* ADDGameModeBase::BlueprintGetSpawnRangeLimiterManager() const 
{
	return SpawnRangeLimiterManager;
}

UDDVillagerpediaWidget* ADDGameModeBase::BlueprintGetVillagerpediaWidget() const
{
	return VillagerpediaWidget;
}

UDDPauseMenuWidget* ADDGameModeBase::BlueprintGetPauseMenuWidget() const
{
	return PauseMenuWidget;
}

UDDSoulShopWidget* ADDGameModeBase::BlueprintGetSoulShopWidget() const
{
	return SoulShopWidget;
}

ADDTheKing* ADDGameModeBase::BlueprintGetTheKing() const
{
	return TheKing;
}

template<class T>
T* ADDGameModeBase::AddWidgetToViewport(TSubclassOf<UUserWidget> WidgetClass, int ZOrder)
{
	if (WidgetClass) {
		T* SomeWidget = Cast<T>(CreateWidget(GetWorld(), WidgetClass));
		if (SomeWidget) {
			SomeWidget->AddToViewport(ZOrder);
			return SomeWidget;
		}
		else {
			UE_LOG(LogTemp, Fatal, TEXT("Error: %s is not a subclass of UUserWidget"), *(SomeWidget->GetName()))
		}
	}
	else {
		UE_LOG(LogTemp, Fatal, TEXT("Error: Invalid WidgetClass argument"))
	}

	return nullptr;
}

template<class T>
T* ADDGameModeBase::FindUObject() const
{
	TArray<AActor*> ActorArray;
	if (UWorld* World = GetWorld()) {
		UGameplayStatics::GetAllActorsOfClass(World, T::StaticClass(), ActorArray);

		check(ActorArray.Num() == 1)
		check(!ActorArray.IsEmpty());

		for (AActor* Actor : ActorArray) {
			T* SomeActor = Cast<T>(Actor);
			if (SomeActor) {
				return SomeActor;
			}
		}
	}
	return nullptr;
}

void ADDGameModeBase::ResetGameData() 
{
	UpdateFinalScoreText();
	TotalEnemiesKilled = 0;
	TotalSouls = 0;
	TotalAcummulatedSouls = 0;
	UpdateScoreText();
	UpdateSoulsText();
	OnPlacing.Broadcast(false);
	bIsPostWave = false;
}

void ADDGameModeBase::SaveGame() const
{
	if (bFullSaveDebug) return; //dont bother saving with the full save debug option enabled

	UDDSaveGame* SaveData = Cast<UDDSaveGame>(UGameplayStatics::CreateSaveGameObject(UDDSaveGame::StaticClass()));
	UDDSaveGame* LoadData = Cast<UDDSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
	SaveData->VillagersDiscovered = VillagerpediaWidget->GetVillagersDiscovered();
	TArray<int32> DifficultiesWonInt;
	for (EDifficulty DifficultyEnum : GetDifficultiesWon()) DifficultiesWonInt.Add((int32)DifficultyEnum);
	SaveData->DifficultiesWon = DifficultiesWonInt;
	if (LoadData) {
		SaveData->WaveHighScore = FMathf::Max(LoadData->WaveHighScore, WaveHighScore);
	}
	else {
		SaveData->WaveHighScore = WaveHighScore;
	}

	if (!UGameplayStatics::SaveGameToSlot(SaveData, SaveSlotName, 0)) {
		UE_LOG(LogTemp, Error, TEXT("Failed to save %s!"), *SaveSlotName)
	}
}
