// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DDGameModeBase.generated.h"

class ADDSpawnRangeLimiterManager;
class UUserWidget;
class UDDScoreWidget;
class UDDMainMenuWidget;
class UDDSoulShopWidget;
class UDDHealthBarWidget;
class UDDNegativeSoulShopWidget;
class UDDVillagerpediaWidget;
class UDDPauseMenuWidget;
class UDDNewVillagerAlertWidget;
class UDDKingAnnouncementsWidget;
class ADDProjectileManager;
class ADDNegativeSoulShopManager;
class ADDEnemySpawner;
class ADDPlaceableManager;
class ADDClickableManager;
class ADDCraterManager;
class ADDDebuffAreaManager;
class ADDPlayer;
class ADDTheKing;
enum class EDifficulty : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlacing, bool, IsPlacing);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameWon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOver);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameRestart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUpdateSouls);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWaveOver);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWaveStart);
//When the player decides to start on a wave that isn't the beginning
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameWaveJumpChoice);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameSave, UDDSaveGame*, SaveGame);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOpenVillagerpedia, UUserWidget*, Widget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPauseChangeState, bool, IsPaused);

UCLASS()
class DRAGONSDEFENSE_V2_API ADDGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> ScoreWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> SoulShopWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> NegativeShopWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> VillagerpediaWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> PauseMenuWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> NewVillagerAlertWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> KingAnnouncementsWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Save")
	FString SaveSlotName;
	//Set to true to start with a 100% save, unlocking everything!
	UPROPERTY(EditDefaultsOnly, Category = "Save")
	bool bFullSaveDebug = false;
	//What wave the final boss card for The King appears
	UPROPERTY(EditDefaultsOnly, Category = "The King")
	int32 TheKingBossWave = 50;

	void UpdateScoreText();
	void UpdateFinalScoreText();
	void UpdateSoulsText();

public:

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	void AddScore(int32 Score);
	void UpdateSouls(int32 Souls);

	ADDProjectileManager& GetProjectileManager();
	ADDPlaceableManager& GetPlaceableManager();
	ADDEnemySpawner& GetEnemySpawner();
	ADDSpawnRangeLimiterManager& GetSpawnRangeLimiterManager();
	ADDPlayer& GetPlayer();
	ADDClickableManager& GetClickableManager();
	ADDCraterManager& GetCraterManager();
	ADDDebuffAreaManager& GetDebuffAreaManager();
	UDDNegativeSoulShopWidget& GetNegativeSoulShopWidget();
	ADDNegativeSoulShopManager& GetNegativeSoulShopManager();
	UDDNewVillagerAlertWidget& GetNewVillagerAlertWidget();
	UDDSoulShopWidget& GetSoulShopWidget();
	ADDTheKing& GetTheKing();

	UFUNCTION(BlueprintPure, Category = "Getters")
	const EDifficulty GetDifficulty() const;
	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetDifficulty(const EDifficulty NewDifficulty);
	UFUNCTION(BlueprintPure, Category = "Getters")
	const int32 GetSouls() const;
	UFUNCTION(BlueprintPure, Category = "Getters")
	const int32 GetAcummulatedSouls() const;
	//Get the soul amount at the beginning of the wave
	UFUNCTION(BlueprintPure, Category = "Getters")
	const int32 GetBeginningSouls() const;
	UFUNCTION(BlueprintPure, Category = "Getters")
	UDDMainMenuWidget* GetMainMenuWidget() const;
	UFUNCTION(BlueprintPure, Category = "Getters")
	const bool GetPauseState() const;
	//Returns true if post wave
	//Returns false if during wave
	UFUNCTION(BlueprintPure, Category = "Getters")
	const bool GetPostWaveState() const;
	//Returns list of difficulties that has been won
	UFUNCTION(BlueprintPure, Category = "Getters")
	const TArray<EDifficulty> GetDifficultiesWon() const;
	UFUNCTION(BlueprintPure, Category = "Getters")
	const int32 GetTheKingBossWave() const;
	UFUNCTION(BlueprintCallable, Category = "Setters")
	void SetPauseState(const bool PauseState);
	UFUNCTION(BlueprintCallable, Category = "GameState")
	void GameWon();
	UFUNCTION(BlueprintCallable, Category = "GameState")
	void GameOver();
	UFUNCTION(BlueprintCallable, Category = "GameState")
	void GameStart() const;
	UFUNCTION(BlueprintCallable, Category = "GameState")
	void GameRestart();
	UFUNCTION(BlueprintCallable, Category = "WaveState")
	void WaveOver();
	UFUNCTION(BlueprintCallable, Category = "WaveState")
	void WaveStart();
	UFUNCTION(BlueprintCallable, Category = "WaveState")
	void IsPlacing(bool Placing) const;

	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnGameWon OnGameWon;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnGameOver OnGameOver;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnGameStart OnGameStart;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnUpdateSouls OnUpdateSouls;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnWaveStart OnWaveStart;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnWaveOver OnWaveOver;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnPlacing OnPlacing;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnOpenVillagerpedia OnOpenVillagerpedia;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnGameRestart OnGameRestart;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnPauseChangeState OnPauseChangeState;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnGameWaveJumpChoice OnGameWaveJumpChoice;
	UPROPERTY(BlueprintAssignable, Category = "Delegate")
	FOnGameSave OnGameSave;

private:
	//Adds any subclass of UUserWidget to the viewport. Returns whatever class
	//that inherits from UUserWidget.
	template <class T> T* AddWidgetToViewport(TSubclassOf<UUserWidget> WidgetClass, int ZOrder = 0);
	//Searches for some kind of UObject in the world based on the class passed in. 
	// Also checks if there is only one instance of the object.
	template <class T> T* FindUObject() const;
	void ExecuteOpenVillagerpediaDelegate(UUserWidget* Widget);
	void ResetGameData();
	void SaveGame() const;

	//Blueprint functions below are so C++ can't call it but blueprints can
	//since C++ can modify the pointers (even though you can change by reference
	//to pointer but shut up okay?)

	UFUNCTION(BlueprintCallable, Category = "Getters")
	ADDPlayer* BlueprintGetPlayer() const;
	UFUNCTION(BlueprintCallable, Category = "Getters")
	ADDProjectileManager* BlueprintGetProjectileManager() const;
	UFUNCTION(BlueprintCallable, Category = "Getters")
	ADDPlaceableManager* BlueprintGetPlaceableManager() const;
	UFUNCTION(BlueprintCallable, Category = "Getters")
	ADDNegativeSoulShopManager* BlueprintGetNegativeSoulShopManager();
	UFUNCTION(BlueprintCallable, Category = "Getters")
	UDDNegativeSoulShopWidget* BlueprintGetNegativeSoulShopWidget() const;
	UFUNCTION(BlueprintCallable, Category = "Getters")
	ADDEnemySpawner* BlueprintGetEnemySpawner() const;
	UFUNCTION(BlueprintCallable, Category = "Getters")
	ADDSpawnRangeLimiterManager* BlueprintGetSpawnRangeLimiterManager() const;
	UFUNCTION(BlueprintCallable, Category = "Getters")
	UDDVillagerpediaWidget* BlueprintGetVillagerpediaWidget() const;
	UFUNCTION(BlueprintCallable, Category = "Getters")
	UDDPauseMenuWidget* BlueprintGetPauseMenuWidget() const;
	UFUNCTION(BlueprintCallable, Category = "Getters")
	UDDSoulShopWidget* BlueprintGetSoulShopWidget() const;
	UFUNCTION(BlueprintCallable, Category = "Getters")
	ADDTheKing* BlueprintGetTheKing() const;

	UFUNCTION()
	void WaveJumpChoiceEventFunction() const;

	ADDProjectileManager* ProjectileManager;
	ADDPlaceableManager* PlaceableManager;
	ADDNegativeSoulShopManager* NegativeShopManager;
	ADDEnemySpawner* EnemySpawner;
	ADDSpawnRangeLimiterManager* SpawnRangeLimiterManager;
	ADDPlayer* Player;
	ADDClickableManager* ClickableManager;
	ADDCraterManager* CraterManager;
	ADDDebuffAreaManager* DebuffAreaManager;
	ADDTheKing* TheKing;

	UDDScoreWidget* ScoreWidget;
	UDDMainMenuWidget* MainMenuWidget;
	UDDSoulShopWidget* SoulShopWidget;
	UDDHealthBarWidget* HealthBarWidget;
	UDDNegativeSoulShopWidget* NegativeShopWidget;
	UDDVillagerpediaWidget* VillagerpediaWidget;
	UDDPauseMenuWidget* PauseMenuWidget;
	UDDNewVillagerAlertWidget* NewVillagerAlertWidget;
	UDDKingAnnouncementsWidget* KingAnnouncementsWidget;

	EDifficulty Difficulty;

	//bool determines if the difficulty has been won
	TMap<EDifficulty, bool> DifficultiesWon;
	TMap<EDifficulty, int32> DifficultyWaveHighScores;

	const int32 LARGE_WAVE_HIGH_SCORE = 9999999;

	int32 WaveHighScore = 1;

	int32 TotalEnemiesKilled = 0;
	int32 TotalSouls = 0;
	int32 TotalAcummulatedSouls = 0;
	int32 BeginningSoulAmount = 0;

	bool bIsPaused = false;
	bool bIsPostWave = false;
};