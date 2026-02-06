// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDTheKing.h"
#include "Components/BillboardComponent.h"
//My classes
#include "DDSpawnRangeLimiterManager.h"
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/DDPlaceableManager.h"
#include "DragonsDefense_v2/Game/DDNegativeSoulShopManager.h"
#include "DragonsDefense_v2/Game/DDDifficulty.h"
#include "DragonsDefense_v2/Game/DDStressMeterTypes.h"
#include "DragonsDefense_v2/Characters/DDPlaceable.h"
#include "DragonsDefense_v2/Characters/DDHorizontalEnemy.h"
#include "DragonsDefense_v2/Characters/DDDragonkinSentient.h"
#include "DragonsDefense_v2/Characters/DDCraterCreator.h"
#include "DragonsDefense_v2/Characters/DDPlayerStats.h"
#include "DragonsDefense_v2/Characters/DDEnemyTypes.h"
#include "DragonsDefense_v2/Helpers/DDOccupationSquare.h"
#include "DragonsDefense_v2/UI/DDNegativeSoulShopWidget.h"
#include "DragonsDefense_v2/UI/DDUpgradeShopButton.h"
#include "DragonsDefense_v2/UI/DDSoulShopWidget.h"

//Returns a correctly rounded integer, depending on if its decimal part is greater than 0.5 or not
inline static int32 CorrectlyRoundedFloatToInt(const float Number) {
	return (Number - FMathf::Floor(Number)) >= 0.5f ? FMathf::Ceil(Number) : FMathf::Floor(Number);
}

// Sets default values
ADDTheKing::ADDTheKing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	KingIcon = CreateDefaultSubobject<UBillboardComponent>("TheKingIcon");

	RootComponent = KingIcon;

	OriginalGoldenEnemyChance = GoldenEnemyChance;
}

// Called when the game starts or when spawned
void ADDTheKing::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->OnGameStart.AddDynamic(this, &ADDTheKing::GameStartEventFunction);
		GameMode->OnGameOver.AddDynamic(this, &ADDTheKing::GameOverEventFunction);
		GameMode->OnGameRestart.AddDynamic(this, &ADDTheKing::GameOverEventFunction);
		GameMode->OnGameWon.AddDynamic(this, &ADDTheKing::GameOverEventFunction);
		GameMode->OnWaveOver.AddDynamic(this, &ADDTheKing::WaveOverEventFunction);
		GameMode->OnGameWaveJumpChoice.AddDynamic(this, &ADDTheKing::GameWaveJumpChoiceEventFunction);
	}
	else {
		UE_LOG(LogTemp, Fatal, TEXT("Gamemode was not found! Killing application..."))
	}

	InitializeOccupationSquares();
	ThreatLevelPerColumn.Init(0.0f, OccupationSquareAmount.Y);
	//Theres some initialization that can be done within this function
	ResetVariables();
}

void ADDTheKing::Tick(float DeltaTime) 
{
	Super::Tick(DeltaTime);

	if (GameMode && !GameMode->GetPostWaveState() && CastleStressBucketLevel > 0.0f) {
		UpdateCastleStressBucket(DeltaTime);
	}
}

TArray<AActor*> ADDTheKing::GetOccupationSquares() const
{
	TArray<AActor*> Squares;
	for (TArray<ADDOccupationSquare*> SquareArray : OccupationSquares) {
		for (ADDOccupationSquare* Square : SquareArray) {
			Squares.Add(Square);
		}
	}

	return Squares;
}

void ADDTheKing::AddDowngradeFunction(std::function<void()> Func)
{
	DowngradeFunctions.Add(Func);
}

FDDWaveInfo ADDTheKing::GetCalculatedWaveInfo() const
{
	return CalculatedWaveInfo;
}

void ADDTheKing::SetCalculatedWaveInfo(const FDDWaveInfo WaveInfo)
{
	CalculatedWaveInfo = WaveInfo;
}

void ADDTheKing::UpdateCastleStressBucket(float DeltaTime) 
{
	BucketDrainTimer += DeltaTime;
	if (BucketDrainTimer >= CastleStressBucketDrainDelay) {
		if (!bIsTempBucketLevelSet) {
			TempCastleStressBucketLevel = CastleStressBucketLevel;
			TempStressLevelWithoutBucket = CurrentStressLevel - CastleStressBucketLevel;
			bIsTempBucketLevelSet = true;
		}
		if (bHasStressLevelChanged) {
			TempStressLevelWithoutBucket = CurrentStressLevel - CastleStressBucketLevel;
			bHasStressLevelChanged = false;
		}

		TempCastleStressBucketDrainTime += DeltaTime;

		const float LerpStressBucketLevel = FMathf::Clamp(FMath::Lerp(TempCastleStressBucketLevel, 
																			0.0f, 
																			(TempCastleStressBucketDrainTime / TotalCastleStressBucketDrainTime)), 
														0.0f, 
														TempCastleStressBucketLevel);

		CastleStressBucketLevel = LerpStressBucketLevel;
		const float TestStressLevel = TempStressLevelWithoutBucket + LerpStressBucketLevel;
		if (TestStressLevel < 0) {
			UE_LOG(LogTemp, Error, TEXT("Stress level drain went below 0!"))
		}
		CurrentStressLevel = FMathf::Max(TestStressLevel, 0.0f);

		OnStressChange.Broadcast(CurrentStressLevel, ActualMaxStressMeterThreshold);
	}
}

void ADDTheKing::CheckGoldenEnemyAppearance()
{
	float RandomNumber = FMath::RandRange(0.0f, 100.0f);

	if (RandomNumber <= GoldenEnemyChance) {
		CalculatedWaveInfo.EnemiesToSpawn.Add(EnemyEncyclopediaRef[EDDEnemyIDs::GOLDEN_ENEMY]);
		GoldenEnemyChance = OriginalGoldenEnemyChance;
	}
	else {
		GoldenEnemyChance += GoldenEnemyChanceIncrease;
	}
}

void ADDTheKing::CheckDisrupterAppearance()
{
	TMap<EDDEnemyIDs, TSubclassOf<AEnemy>> EnemyEncyclopedia = EnemySpawner->GetEnemyEncyclopedia();

	if (EnemySpawner->GetCurrentWave() % DisrupterAppearanceWave == 0) {
		CalculatedWaveInfo.EnemiesToSpawn.Add(EnemyEncyclopediaRef[EDDEnemyIDs::DISRUPTER]);
		bIsDisrupterHere = true;
	}
}

void ADDTheKing::CheckSpecialWave()
{
	//insert maybe several types of special wave events?
	if (EnemySpawner->GetCurrentWave() >= SpecialWaveNumber) {
		//do something!
		IsSpecialWave = true;
		SpecialWaveNumber += FMath::RandRange(LowerSpecialWaveRandomizer, HigherSpecialWaveRandomizer);
		UE_LOG(LogTemp, Log, TEXT("Special wave occured!"));
	}
	else {
		IsSpecialWave = false;
	}
}

void ADDTheKing::CalculatePlayerStyle()
{
	FDDSecretStats TotalPlayerSecretStats;
	
	TotalPlayerSecretStats += CalculatePlayerUpgradeStats();
	TotalPlayerSecretStats += CalculatePlaceablesStats();

	PlayerCalculatedStats = TotalPlayerSecretStats;

	FVector StatVector = FVector(TotalPlayerSecretStats.Offense, 
								TotalPlayerSecretStats.Defense,
								TotalPlayerSecretStats.Support);

	StatVector.Normalize();
	SecretStats.Style = DeterminePlayerStyle(StatVector);
	SecretStats.CalculatedOffense = StatVector.X;
	SecretStats.CalculatedDefense = StatVector.Y;
	SecretStats.CalculatedSupport = StatVector.Z;
	UE_LOG(LogTemp, Log, TEXT("Secrest player stats: %lf, %lf, %lf"), StatVector.X, StatVector.Y, StatVector.Z);
}

FDDSecretStats ADDTheKing::CalculatePlayerUpgradeStats()
{
	FDDSecretStats TotalStats;
	const TMap<EPlayerStats, TArray<UDDUpgradeShopButton*>> ButtonUpgrades = SoulShopWidget->GetUpgradeButtonsByCategory();

	//This is double for-loop but the amount of buttons and upgrades won't impact performance that much
	for (TPair<EPlayerStats, TArray<UDDUpgradeShopButton*>> ButtonPair : ButtonUpgrades) {
		UDDUpgradeShopButton* Button = ButtonPair.Value[0]; //Only need the first one
		FDDSecretStats CurrentStat;
		//Total up the amount bought in this specific stat
		for (int i = 0; i < Button->GetPriceIndex(); i++) {
			CurrentStat += Button->GetSecretStats();
		}
		TotalStats += CurrentStat;
	}

	CachedPlayerSecretStatsFromStats = TotalStats;
	return TotalStats;
}

FDDSecretStats ADDTheKing::CalculatePlaceablesStats()
{
	FDDSecretStats Stats;
	TArray<ADDPlaceable*> PlaceablePool = PlaceableManager->GetPlaceablePool();
	for (ADDPlaceable* Placeable : PlaceablePool) {
		if (Placeable->IsA<ADDDragonkinSentient>()) {
			//Dragon-kin inherits player's stats so this is how it will be 
			//done unless it sways the normalization too much
			Stats += CachedPlayerSecretStatsFromStats;
		}
		else {
			Stats += Placeable->GetSecretStats();
		}
	}
	return Stats;
}

EDDPlayerStyle ADDTheKing::DeterminePlayerStyle(FVector Stats)
{
	//Damn this is some ugly ass code
	//X is offense, Y is defense, Z is support
	if (Stats.X > Stats.Y && Stats.X > Stats.Z)
	{
		return EDDPlayerStyle::Aggressive;
	}
	else if (Stats.Y > Stats.X && Stats.Y > Stats.Z)
	{
		return EDDPlayerStyle::Defensive;
	}
	else if (Stats.Z > Stats.X && Stats.Z > Stats.Y)
	{
		return EDDPlayerStyle::Supportive;
	}
	else
	{
		return EDDPlayerStyle::Generalist;
	}
}

void ADDTheKing::CalculateWellnessPoints()
{
	const EDifficulty CurrentDifficulty = GameMode->GetDifficulty();
	const int32 TotalSouls = GameMode->GetAcummulatedSouls();
	const int32 CurrentSouls = GameMode->GetSouls();
	int32 BeginningSouls = GameMode->GetBeginningSouls();
	const float CurrentPlayerHealth = Player->GetHealth();
	const float TotalPlayerHealth = Player->GetUnModdedMaxHealth();
	const int32 CurrentPlaceableAmount = PlaceableManager->GetPlaceablePool().Num();
	const int32 TotalPlaceablesPlaced = PlaceableManager->GetTotalPlaceablesPlaced();
	const int32 BeginningPlaceableAmount = PlaceableManager->GetBeginningPlaceableAmount();
	const int32 WaveNumber = EnemySpawner->GetCurrentWave();
	const bool IsDowngradeActive = NegativeSoulShopManager->GetIsDowngradeActive();
	const bool IsRewardActive = NegativeSoulShopManager->GetIsRewardActive();
	const TMap<EPlayerStats, TArray<UDDUpgradeShopButton*>> ButtonUpgrades = SoulShopWidget->GetUpgradeButtonsByCategory();

	float ButtonSouls = 0;

	/* TODO - Things to consider:
	* Consider not the total for the whole game, but the total from the previous wave for whatever variable keeps track of total over the game
	* prob some addition to the previous comment, not exactly sure what, but some mediator
	*/

	for (TPair <EPlayerStats, TArray<UDDUpgradeShopButton*>> ButtonPair : ButtonUpgrades) {
		UDDUpgradeShopButton* Butt = ButtonPair.Value[0];
		const int32 CurrentIndex = Butt->GetPriceIndex();
		if (CurrentIndex > 0) {
			ButtonSouls += Butt->GetPrefixSumPrice(Butt->GetPriceIndex() - 1);
		}
	}

	BeginningSouls += ButtonSouls;

	//Lets avoid dividing by 0 shall we?
	const float SoulRatio = (BeginningSouls == 0) ? CurrentSouls : static_cast<float>(CurrentSouls) / BeginningSouls;
	UE_LOG(LogTemp, Log, TEXT("Soul Ratio: %f"), SoulRatio);
	const float CorrectedSoulRatio = FMath::Clamp(SoulRatio, 0.01f, INFINITY);
	UE_LOG(LogTemp, Log, TEXT("Corrected Soul Ratio: %f"), CorrectedSoulRatio);
	//If current placeable soul amount is higher than beginning soul amount, a penalty is put on pity points
	//Otherwise, pity points will be increased
	// TODO - maybe adjust the 1 to a lower value if ratio is too harsh on pity points?
	const float SoulRatioEffect = FMath::Clamp(1 / CorrectedSoulRatio, 0.0f, HighPityEffectLimit);
	UE_LOG(LogTemp, Log, TEXT("Soul Ratio Effect: %f"), SoulRatioEffect);

	const float PlaceableRatio = (BeginningPlaceableAmount == 0) ? 0 : static_cast<float>(CurrentPlaceableAmount) / BeginningPlaceableAmount;
	UE_LOG(LogTemp, Log, TEXT("Placeable Ratio: %f"), PlaceableRatio);
	const float CorrectedPlaceableRatio = FMath::Clamp(PlaceableRatio, 0.01f, INFINITY);
	UE_LOG(LogTemp, Log, TEXT("Corrected Placeable Ratio: %f"), CorrectedPlaceableRatio);
	//If current placeable amount is higher than beginning placeable amount, a penalty is put on pity points
	//Otherwise, pity points will be increased
	// TODO - maybe adjust the 1 to a lower value if ratio is too harsh on pity points?
	const float PlaceableRatioEffect = FMath::Clamp(1 / CorrectedPlaceableRatio, 0.0f, HighPityEffectLimit);
	UE_LOG(LogTemp, Log, TEXT("PlaceableRatioEffect: %f"), PlaceableRatioEffect);

	const float HealthRatio = CurrentPlayerHealth / TotalPlayerHealth;
	UE_LOG(LogTemp, Log, TEXT("HealthRatio : %f"), HealthRatio);

	StabilityPoints = (BeginningSouls * BeginningSoulsFactor) + (TotalSouls * TotalSoulsFactor) +
						(BeginningPlaceableAmount * BeginningPlaceablesPlacedFactor) + (TotalPlaceablesPlaced * TotalPlaceablesFactor) +
						(TotalPlayerHealth * TotalPlayerHealthFactor) +
						(WaveNumber * WaveNumberFactor);
	UE_LOG(LogTemp, Log, TEXT("StabilityPoints : %f"), StabilityPoints);

	float CurrentDifficultyFactor = 1.0f;

	switch (CurrentDifficulty) {
		case EDifficulty::Easy:
			CurrentDifficultyFactor = EasyFactor;
			break;
		case EDifficulty::Normal:
			//do nothing
			break;
		case EDifficulty::Hard:
			CurrentDifficultyFactor = HardFactor;
			break;
		default:
			CurrentDifficultyFactor = 1.0f;
			break;
	}
	UE_LOG(LogTemp, Log, TEXT("CurrentDifficultyFactor : %f"), CurrentDifficultyFactor);

	float DowngradeRewardFactor = 1.0f;
	if (IsDowngradeActive) {
		DowngradeRewardFactor = DowngradeFactor;
	}
	else if (IsRewardActive) {
		DowngradeRewardFactor = RewardFactor;
	}
	UE_LOG(LogTemp, Log, TEXT("DowngradeRewardFactor : %f"), DowngradeRewardFactor);

	float CalculatedPityPoints = (((CurrentPlaceableAmount * CurrentPlaceableAmountFactor) * PlaceableRatioEffect) +
									((TotalPlayerHealth + WaveNumber) * (1.0 - HealthRatio)) +
									((CurrentSouls * CurrentSoulsFactor) * SoulRatioEffect)) *
									CurrentDifficultyFactor *
									DowngradeRewardFactor;
	UE_LOG(LogTemp, Log, TEXT("CalculatedPityPoints : %f"), CalculatedPityPoints);

	if (CalculatedPityPoints > StabilityPoints) {
		UE_LOG(LogTemp, Warning, TEXT("Pity Points exceeded Stability Points! You might want to check your formula! Results: Pity - %f\tStability - %f"))
	}

	//Limiting pity points to avoid exceeding stability points, and also not give too much pity
	//Pity points will get lower the higher the wave number, making The King more aggressive in later waves
	PityPoints = CorrectlyRoundedFloatToInt(FMathf::Clamp(CalculatedPityPoints * (StabilityPoints / (FMathf::Clamp(StabilityPoints + FMath::Pow(WaveNumberFactor, PityPointExponentLimiter), 1, INFINITY))),
													0.0f,
													StabilityPoints));

	SafetyPoints = StabilityPoints - PityPoints;
}

void ADDTheKing::CalculateEnemyPoolSize()
{
	const EDifficulty CurrentDifficulty = GameMode->GetDifficulty();

	int32 ActualInitialMaxWaveNumber = InitialMaxWaveNumber;

	switch (CurrentDifficulty) {
		case EDifficulty::Easy:
			ActualInitialMaxWaveNumber += EasyWaveNumberModifier;
			break;
		case EDifficulty::Normal:
			//Nothing
			break;
		case EDifficulty::Hard:
			ActualInitialMaxWaveNumber -= HardWaveNumberModifier;
			break;
	}
	UE_LOG(LogTemp, Log, TEXT("ActualInitialMaxWaveNumber : %d"), ActualInitialMaxWaveNumber);

	const int32 WaveNumber = EnemySpawner->GetCurrentWave();
	//Player always starts at wave 1 after all!
	const int32 MinWaveNumber = 1;
	//We do exponential decay in this motherfucker! better take your sensitive ass back to linear decreasing!
	const float MaxWaveNumberFloat = WaveNumber + (ActualInitialMaxWaveNumber - WaveNumber) * FMathf::Pow(e, -ExponentialDecayExponent * WaveNumber);
	UE_LOG(LogTemp, Log, TEXT("MaxWaveNumberFloat : %f"), MaxWaveNumberFloat);
	//Correctly round this guy!
	const int32 MaxWaveNumber = CorrectlyRoundedFloatToInt(MaxWaveNumberFloat);
	UE_LOG(LogTemp, Log, TEXT("MaxWaveNumber : %d"), MaxWaveNumber);

	const float NormalizedWaveNumber = static_cast<float>((WaveNumber - MinWaveNumber)) / static_cast<float>((MaxWaveNumber - MinWaveNumber));
	UE_LOG(LogTemp, Log, TEXT("NormalizedWaveNumber : %f"), NormalizedWaveNumber);
	const float NormalizedSafetyPoints = static_cast<float>(SafetyPoints) / static_cast<float>(StabilityPoints);
	UE_LOG(LogTemp, Log, TEXT("NormalizedSafetyPoints : %f"), NormalizedSafetyPoints);

	const float SafetyPointsWeight = 1.0f - WaveNumberWeight;
	UE_LOG(LogTemp, Log, TEXT("SafetyPointsWeight : %f"), SafetyPointsWeight);

	//Normalized safety points contributes to adding one more to the enemy pool size
	EnemyPoolSize = FMath::Clamp(CorrectlyRoundedFloatToInt((MaxEnemyPoolSize * NormalizedWaveNumber * WaveNumberWeight) + NormalizedSafetyPoints), 1, MaxEnemyPoolSize);
	UE_LOG(LogTemp, Log, TEXT("EnemyPoolSize : %d"), EnemyPoolSize);

	//TODO - this aint it chief
	if (IsSpecialWave) {
		EnemyPoolSize += FMath::RandRange(1, 4);
	}
}

void ADDTheKing::DetermineEnemyPool()
{
	//TODO - think of certain special events skip these pieces maybe?
	TPair<int32, int32> EnemyTypesAmount = DetermineEnemyTypesInPool();
	//Determining basic enemies first
	DetermineEnemies(CalculatedWaveInfo.EnemiesToSpawn, BasicEnemyArray, EnemyTypesAmount.Key);
	TArray<TSubclassOf<AEnemy>> BigSpecialEnemyArray = SpecialEnemyArray;
	if (EnemySpawner->GetCurrentWave() >= AttackingSpecialEnemyWave) {
		BigSpecialEnemyArray.Append(AttackingSpecialEnemyArray);
	}
	//Determining special enemies after
	SpecialEnemyLeftovers = DetermineEnemies(CalculatedWaveInfo.EnemiesToSpawn, BigSpecialEnemyArray, EnemyTypesAmount.Value);
}

TPair<int32, int32> ADDTheKing::DetermineEnemyTypesInPool() const
{
	float RandomValue;
	float DifficultyBasicEnemyWeightMod = 1.0f;
	int32 ActualInitialMaxWaveNumber = 50;
	int32 BasicEnemyCounter = 0;
	int32 SpecialEnemyCounter = 0;
	int32 SpecialEnemyLimitWaveNumber = 1;

	EDifficulty CurrentDifficulty = GameMode->GetDifficulty();

	switch (CurrentDifficulty) {
		case EDifficulty::Easy:
			DifficultyBasicEnemyWeightMod = EasyBasicEnemyWeightModifier;
			ActualInitialMaxWaveNumber += EasyWaveNumberModifier;
			SpecialEnemyLimitWaveNumber = SpecialEnemyLimitEasyWaveNumberThreshold;
			break;
		case EDifficulty::Normal:
		default:
			DifficultyBasicEnemyWeightMod = 1.0f;
			SpecialEnemyLimitWaveNumber = SpecialEnemyLimitNormalWaveNumberThreshold;
			break;
		case EDifficulty::Hard:
			DifficultyBasicEnemyWeightMod = HardBasicEnemyWeightModifier;
			ActualInitialMaxWaveNumber -= HardWaveNumberModifier;
			SpecialEnemyLimitWaveNumber = SpecialEnemyLimitHardWaveNumberThreshold;
			break;
	}
	UE_LOG(LogTemp, Log, TEXT("ActualInitialMaxWaveNumber : %d"), ActualInitialMaxWaveNumber);

	//Note for self - I do exponentital decay on MaxWaveNumberFloat in order to decrease it
	//as waves go on, increasing the weight of special enemy

	const int32 WaveNumber = EnemySpawner->GetCurrentWave();
	//Player always starts at wave 1 after all!
	const int32 MinWaveNumber = 1;
	//We do exponential decay in this motherfucker! better take your sensitive ass back to linear decreasing!
	const float MaxWaveNumberFloat = WaveNumber + (ActualInitialMaxWaveNumber - WaveNumber) * FMathf::Pow(e, -ExponentialDecayExponent * WaveNumber);
	UE_LOG(LogTemp, Log, TEXT("MaxWaveNumberFloat : %f"), MaxWaveNumberFloat);
	//Correctly round this guy!
	const int32 MaxWaveNumber = CorrectlyRoundedFloatToInt(MaxWaveNumberFloat);
	UE_LOG(LogTemp, Log, TEXT("MaxWaveNumber : %d"), MaxWaveNumber);

	const float NormalizedWaveNumber = static_cast<float>((WaveNumber - MinWaveNumber)) / static_cast<float>((MaxWaveNumber - MinWaveNumber));
	UE_LOG(LogTemp, Log, TEXT("NormalizedWaveNumber : %f"), NormalizedWaveNumber);

	//TODO - consider trying to nerf and buff that base when waves go on!
	float CalculatedBasicEnemyWeight = BaseBasicEnemyWeight *
										DifficultyBasicEnemyWeightMod *
										(BaseBasicEnemyPityPoints + (PityPoints / StabilityPoints));
	UE_LOG(LogTemp, Log, TEXT("CalculatedBasicEnemyWeight : %f"), CalculatedBasicEnemyWeight);
	float CalculatedSpecialEnemyWeight = BaseSpecialEnemyWeight *
										BaseSpecialEnemyWaveNumber + (NormalizedWaveNumber); //TODO - experiment with safety points to be added here
	UE_LOG(LogTemp, Log, TEXT("CalculatedSpecialEnemyWeight : %f"), CalculatedSpecialEnemyWeight);

	float TotalWeight = 0.0f;

	for (int i = 0; i < EnemyPoolSize; i++) {
		TotalWeight = CalculatedSpecialEnemyWeight + CalculatedBasicEnemyWeight;
		RandomValue = FMath::RandRange(0.0f, 1.0f);
		if (RandomValue < (CalculatedBasicEnemyWeight / TotalWeight) && BasicEnemyCounter < BasicEnemyArray.Num()) {
			CalculatedBasicEnemyWeight *= BasicEnemyDecreaserModifier;
			BasicEnemyCounter++;
		}
		else {
			SpecialEnemyCounter++;
		}
	}
	//Check for if we haven't crossed the special enemy limit threshold yet //TODO - test this functionality
	if (WaveNumber < SpecialEnemyLimitWaveNumber && SpecialEnemyCounter > SpecialEnemyLimit) {
		const int32 EnemyLeftover = SpecialEnemyCounter - SpecialEnemyLimit;
		SpecialEnemyCounter = SpecialEnemyLimit;
		BasicEnemyCounter = FMath::Clamp(BasicEnemyCounter + EnemyLeftover, BasicEnemyCounter, BasicEnemyArray.Num());
	}

	return TPair<int32, int32>(BasicEnemyCounter, SpecialEnemyCounter);
}

TArray<TSubclassOf<AEnemy>> ADDTheKing::DetermineEnemies(TArray<TSubclassOf<AEnemy>>& EnemiesToSpawn, 
														const TArray<TSubclassOf<AEnemy>> EnemyArray, 
														int32 EnemyAmount) const
{
	if (EnemyAmount <= 0) return EnemyArray;

	int32 RandomEnemyIndex = 0;
	//Variables that determine how much weight an enemy will be chosen based
	//On how well they counter the player's style
	float OffenseWeight, DefenseWeight, SupportWeight;

	float TrueIndexBiasStrength = IndexBiasStrength;
	//The higher the rate (out of 1), the more the player is failing
	float PlayerSuccessRate = PityPoints / (float) StabilityPoints;
	TArray<TSubclassOf<AEnemy>> TempEnemyArray = EnemyArray;

	switch (SecretStats.Style) {
		case EDDPlayerStyle::Aggressive:
			OffenseWeight = EqualCounterWeight;
			DefenseWeight = CompleteCounterWeight;
			SupportWeight = NoCounterWeight;
			break;
		case EDDPlayerStyle::Defensive:
			OffenseWeight = NoCounterWeight;
			DefenseWeight = EqualCounterWeight;
			SupportWeight = CompleteCounterWeight;
			break;
		case EDDPlayerStyle::Supportive:
			OffenseWeight = CompleteCounterWeight;
			DefenseWeight = NoCounterWeight;
			SupportWeight = EqualCounterWeight;
			break;
		case EDDPlayerStyle::Generalist:
		default:
			OffenseWeight = EqualCounterWeight;
			DefenseWeight = EqualCounterWeight;
			SupportWeight = EqualCounterWeight;
			break;
	}

	//Sorted array of how well an enemy counters the player's style
	//High index means high counter, close to 0 is no counter
	TArray<TPair<float, TSubclassOf<AEnemy>>> EnemyCounterArray;

	float TotalCounter = 0;

	FVector SecretStatsVector = FVector(SecretStats.CalculatedOffense, SecretStats.CalculatedDefense, SecretStats.CalculatedSupport) * 100.0f;

	for (TSubclassOf<AEnemy> EnemyClass : EnemyArray) {
		FDDSecretStats EnemyStats = EnemyClass.GetDefaultObject()->GetSecretStats();

		FVector EnemyVector = FVector(EnemyStats.Offense, EnemyStats.Defense, EnemyStats.Support);

		FVector StatDifference = SecretStatsVector - EnemyVector;
		
		TotalCounter = (StatDifference.X * OffenseWeight)
					+ (StatDifference.Y * DefenseWeight)
					+ (StatDifference.Z * SupportWeight);

		EnemyCounterArray.Add(TPair<float, TSubclassOf<AEnemy>>(TotalCounter, EnemyClass));
		TotalCounter = 0;
	}

	//Sort based on the TotalCounter key
	EnemyCounterArray.Sort([](const TPair<float, TSubclassOf<AEnemy>> A, const TPair<float, TSubclassOf<AEnemy>> B) {
						return A.Key < B.Key;
						});

	if (PlayerSuccessRate <= PlayerSuccessThreshold) {
		TrueIndexBiasStrength = IndexBiasStrength + 0.1f;
	}
	else if (PlayerSuccessRate >= PlayerFailureThreshold) {
		TrueIndexBiasStrength = IndexBiasStrength - 0.15f;
	}

	float RandomIndex, BiasedIndex;
	int32 CorrectedIndex, MidIndex;

	for (int32 i = 0; i < EnemyAmount; i++) {
		RandomIndex = FMath::RandRange(0.0f, (float)(TempEnemyArray.Num() - 1));

		//Checking since being less than 1 further reduces RandomIndex due the Pow function
		if (RandomIndex >= 1) {
			//Power is used to influence the randomization to the higher end of the counter array
			BiasedIndex = FMath::Clamp(FMath::Pow(RandomIndex, TrueIndexBiasStrength), 0.0f, TempEnemyArray.Num() - 1);
		}
		else {
			BiasedIndex = RandomIndex;
		}

		CorrectedIndex = CorrectlyRoundedFloatToInt(BiasedIndex);
		MidIndex = CorrectlyRoundedFloatToInt((TempEnemyArray.Num() - 1) / 2.0f);

		switch (GameMode->GetDifficulty()) {
			case EDifficulty::Easy:
				//If the index that was chosen lands higher up on the enemy counter array, this math
				//lowers the odds of getting a high counter enemy
				TrueIndexBiasStrength -= FMath::Clamp((CorrectedIndex - MidIndex / (float)(TempEnemyArray.Num() - (MidIndex + 1))) * EasyEnemyCounterBiasAdjuster, 0, 1.0f);
				break;
			case EDifficulty::Hard:
				//The opposite of the above comment
				TrueIndexBiasStrength += FMath::Clamp((MidIndex - CorrectedIndex / (float)(TempEnemyArray.Num() - (MidIndex + 1))) * HardEnemyCounterBiasAdjuster, 0, 1.0f);
				break;
		}

		EnemiesToSpawn.Add(TempEnemyArray[CorrectedIndex]);
		TempEnemyArray.RemoveAt(CorrectedIndex);
	}

	return TempEnemyArray;
}

void ADDTheKing::CalculateEnemyAmount()
{
	const int32 PreviousEnemyAmount = EnemySpawner->GetMaxEnemySpawn();
	const int32 CurrentWave = EnemySpawner->GetCurrentWave();

	float DifficultyWaveEnemyAmountModifier = 1.0f;

	switch (GameMode->GetDifficulty()) {
		case EDifficulty::Easy:
			DifficultyWaveEnemyAmountModifier = EasyWaveEnemyAmountModifier;
			break;
		case EDifficulty::Normal:
			//Nothing
			break;
		case EDifficulty::Hard:
			DifficultyWaveEnemyAmountModifier = HardWaveEnemyAmountModifier;
			break;
	}

	int32 CalculatedEnemyAmount = PreviousEnemyAmount;
	//Basically a quadratic formula, but reduces the explosive growth
	CalculatedEnemyAmount += (FMath::Pow((CurrentWave * EnemyAmountWaveFactor) / EnemyAmountDivider, 2)
							* DifficultyWaveEnemyAmountModifier) +
							BaseEnemyAmount;

	CalculatedWaveInfo.NumberOfEnemies = CalculatedEnemyAmount;
}

void ADDTheKing::CheckWildCardAppearance()
{
	const int32 CurrentWave = EnemySpawner->GetCurrentWave();
	if (CurrentWave >= CurrentWildCardWave) {
		CurrentWildCardWave += FMath::RandRange(LowWildCardAddition, HighWildCardAddition);
		//Add a random enemy to to the enemy pool!!!
		int32 RandomEnemyIndex = FMath::RandRange(0, SpecialEnemyLeftovers.Num() - 1);
		CalculatedWaveInfo.EnemiesToSpawn.Add(SpecialEnemyLeftovers[RandomEnemyIndex]);
		UE_LOG(LogTemp, Log, TEXT("Added enemy from wild card: %s"), *(SpecialEnemyLeftovers[RandomEnemyIndex].GetDefaultObject()->GetName()));
	}
}

void ADDTheKing::PityPointRewards()
{
	int32 TempPityPoints = PityPoints;
	MinStressThresholdReward = 0.0f;

	//About to pull a gamer move of the least extensible and most ugly code ever written in the history of mankind in the following lines
	//This can definitely be done through a strategy pattern or something like that but I just want to get this over with at this point
	float TotalWeight = BaseEnemyAmountReductionWeight + 
						EnemyPoolRemovalWeight + 
						StressMeterMinimumIncreaseWeight;

	//It looks disgusting, I am aware
	TArray<float> PityPointRewardWeights = { BaseEnemyAmountReductionWeight,
											BaseEnemyAmountReductionWeight + EnemyPoolRemovalWeight,
											BaseEnemyAmountReductionWeight + EnemyPoolRemovalWeight + StressMeterMinimumIncreaseWeight };

	//For increasing the pity rewards as time goes on, prevents pity rewards being too easy to obtain in late game
	float EnemyReductionPityRewardMultiplier = 1.0f + (EnemyReductionPityRewardMultiplierIncrement * (EnemySpawner->GetCurrentWave() - 1));
	float EnemyPoolPityRewardMultiplier = 1.0f + (EnemyPoolPityRewardMultiplierIncrement * (EnemySpawner->GetCurrentWave() - 1));
	float StressMeterPityRewardMultiplier = 1.0f + (StressMeterPityRewardMultiplierIncrement * (EnemySpawner->GetCurrentWave() - 1));

	int32 EnemiesToReduce, RandomEnemyIndex, ActualEnemyAmountReductionCost, ActualEnemyPoolRemovalCost, ActualStressMeterMinimumIncreaseCost;

	while (TempPityPoints > 0) {
		float RandomWeight = FMath::FRandRange(0.0f, TotalWeight);

		for (int32 i = 0; i < PityPointRewardWeights.Num(); i++) {
			if (RandomWeight < PityPointRewardWeights[i]) {
				switch (i) {
					case 0: //Reduce the amount of enemies to spawn
						EnemiesToReduce = CalculatedWaveInfo.NumberOfEnemies * BaseEnemyAmountReduction;
						//For now, each enemy counts as one pity point
						ActualEnemyAmountReductionCost = EnemiesToReduce + (BaseEnemyAmountReductionCost * EnemyReductionPityRewardMultiplier);

						if (ActualEnemyAmountReductionCost > TempPityPoints) {
							//Can't afford the reduction, time to run away!
							TempPityPoints = 0;
							UE_LOG(LogTemp, Log, TEXT("Cannot afford to reduce enemy amount!"));
							break;
						}

						CalculatedWaveInfo.NumberOfEnemies -= EnemiesToReduce;
						TempPityPoints -= ActualEnemyAmountReductionCost;
						UE_LOG(LogTemp, Log, TEXT("Reduced enemy amount!"));
						break;
					case 1: //Remove an enemy from the pool
						ActualEnemyPoolRemovalCost = EnemyPoolRemovalCost * EnemyPoolPityRewardMultiplier;

						if (EnemyPoolRemovalCost > TempPityPoints) {
							//Can't afford the removal, time to run away!
							TempPityPoints = 0;
							UE_LOG(LogTemp, Log, TEXT("Cannot afford to remove enemy from pool!"));
							break;
						}

						if (CalculatedWaveInfo.EnemiesToSpawn.Num() > 1) {
							//TODO - Maybe remove the least threatening enemy tbh!
							//will do this after figuring out the dynamic programming of choosing enemies
							//no need for dynamic programming, just sort by what counters the most
							RandomEnemyIndex = FMath::RandRange(0, CalculatedWaveInfo.EnemiesToSpawn.Num() - 1);
							CalculatedWaveInfo.EnemiesToSpawn.RemoveAt(RandomEnemyIndex);
							TempPityPoints -= EnemyPoolRemovalCost;
							UE_LOG(LogTemp, Log, TEXT("Removed enemy from pool!"));
						}
						else {
							//Might as well continue the selection. In the most unluckiest chance, infinite loop will happen
							//Which is near impossible
							UE_LOG(LogTemp, Warning, TEXT("Cannot remove any more enemies from pool!"));
						}

						break;
					case 2: //Increase the minimum stress meter threshold
						ActualStressMeterMinimumIncreaseCost = StressMeterMinimumIncreaseCost * StressMeterPityRewardMultiplier;

						if (StressMeterMinimumIncreaseCost > TempPityPoints) {
							//Can't afford the increase, time to run away!
							TempPityPoints = 0;
							UE_LOG(LogTemp, Log, TEXT("Cannot afford to increase minimum stress meter!"));
							break;
						}

						MinStressThresholdReward += FMath::RandRange(MinimumStressMeterIncrease, MaximumStressMeterIncrease);
						TempPityPoints -= StressMeterMinimumIncreaseCost;
						UE_LOG(LogTemp, Log, TEXT("Increased stress meter!"));
						break;
					default:
						//Something went wrong with the randomization of weight to end up here
						TempPityPoints = 0;
						UE_LOG(LogTemp, Error, TEXT("Unable to get any pity point reward!"));
						break;
					}
				break;
			}
		}
	}
}

void ADDTheKing::ResetWaveInfo()
{
	CalculatedWaveInfo.EnemiesToSpawn.Empty();
	CalculatedWaveInfo.HighSpawnInterval = DefaultHighSpawnInterval;
	CalculatedWaveInfo.LowSpawnInterval = DefaultLowSpawnInterval;
	CalculatedWaveInfo.NumberOfEnemies = DefaultNumberOfEnemies;
	CalculatedWaveInfo.Priority = 0;
	CalculatedWaveInfo.WaveTrigger = 0;
}

void ADDTheKing::ResetTempCastleStressBucketVariables() 
{
	bIsTempBucketLevelSet = false;
	bHasStressLevelChanged = true;
	TempCastleStressBucketDrainTime = 0.0f;
	TempCastleStressBucketLevel = 0.0f;
	BucketDrainTimer = 0.0f;
	GetWorld()->GetTimerManager().ClearTimer(BucketDrainDelayHandle);
}

void ADDTheKing::InitializeOccupationSquares()
{
	float TotalXAxisArea = FMath::Abs(OccupationSquareLimitXAxis.X - OccupationSquareLimitXAxis.Y);
	float TotalYAxisArea = OccupationSquareLimitYAxis * 2.0f;

	if (OccupationSquareAmount.X == 0 || OccupationSquareAmount.Y == 0) {
		UE_LOG(LogTemp, Fatal, TEXT("Zero amount defined for Occupation Square amount, please be at least 1. Skipping..."))
		return;
	}

	FVector2D OccupationSquareArea = FVector2D(TotalXAxisArea / OccupationSquareAmount.X, TotalYAxisArea / OccupationSquareAmount.Y);
	FVector2D SquareAreaCenter = FVector2D(OccupationSquareArea / 2.0f);
	FVector2D LowerLeftCornerCoordinate = FVector2D(OccupationSquareLimitXAxis.X, -OccupationSquareLimitYAxis);
	FVector2D CurrentSquareCoordinate, SquareAreaTotal;

	for (int32 i = 0; i < (int32) OccupationSquareAmount.X; i++) {
		TArray<ADDOccupationSquare*> Squares;
		for (int32 k = 0; k < (int32)OccupationSquareAmount.Y; k++) {
			SquareAreaTotal = FVector2D(OccupationSquareArea.X * i, OccupationSquareArea.Y * k);
			CurrentSquareCoordinate = LowerLeftCornerCoordinate + (SquareAreaCenter + SquareAreaTotal);
			ADDOccupationSquare* Square = GetWorld()->SpawnActor<ADDOccupationSquare>(OccupationSquareClass);
			Square->InitializeOccupationSquare(OccupationSquareArea, FVector2D(i, k));
			Square->SetActorLocation(FVector(CurrentSquareCoordinate, 0));
			Square->FetchSideCoordinates();
			Square->SetSecretStatsBases(BaseOffense, BaseDefense, BaseSupport);
			Square->SetSecretStatsScaling(SecretOffenseScaling, SecretDefenseScaling, SecretSupportScaling);
			Square->OnThreatLevelChange.BindUObject(this, &ADDTheKing::OnThreatLevelChange);
			Square->OnDetectEnemyDeath.BindUObject(this, &ADDTheKing::OnSquareDetectEnemyDeath);
			Squares.Add(Square);
		}
		OccupationSquares.Add(Squares);
	}
}

void ADDTheKing::FetchOccupationSquareStats()
{
	int CurrentSquareRow = OccupationSquareAmount.X;
	float ThreatLevelMultiplier = 1 + (ThreatLevelMultiplierScaling * CurrentSquareRow);

	for (TArray<ADDOccupationSquare*> SquareArray : OccupationSquares) {
		for (ADDOccupationSquare* Square : SquareArray) {
			//Refresh squares by resetting to prevent adding up previous stats
			//Square->ResetOccupationSquare();

			//dont know if we need to do all of this stats stuff tbh, just need to set threat level mulitplier
			Square->FetchOccupationStats(ThreatLevelMultiplier);
		}
		ThreatLevelMultiplier -= ThreatLevelMultiplierScaling;
	}
}

void ADDTheKing::InitializeStressMeter()
{
	float TotalUpgradeThreatLevel = 0.0f;

	ActualMinStressMeterThreshold = ActualMinStressMeterThreshold + MinStressThresholdReward;

	const TMap<EPlayerStats, TArray<UDDUpgradeShopButton*>> ButtonUpgrades = SoulShopWidget->GetUpgradeButtonsByCategory();

	for (TPair<EPlayerStats, TArray<UDDUpgradeShopButton*>> ButtonPair : ButtonUpgrades) {
		UDDUpgradeShopButton* Butt = ButtonPair.Value[0];
		const int32 CurrentIndex = Butt->GetPriceIndex();
		if (CurrentIndex > 0) {
			TotalUpgradeThreatLevel += Butt->GetPrefixThreatLevel(CurrentIndex - 1);
		}
	}

	CurrentTotalUpgradeThreatLevel = TotalUpgradeThreatLevel;

	//TODO - Wild card room
}

void ADDTheKing::CalculateThreatLevelsPerColumn()
{
	if (ThreatLevelPerColumn.IsEmpty()) {
		ThreatLevelPerColumn.Init(0.0f, OccupationSquareAmount.Y);
	}

	float TempThreatLevel = 0.0f;

	for (int32 i = 0; i < OccupationSquareAmount.X; i++) {
		for (int32 k = 0; k < OccupationSquareAmount.Y; k++) {
			TempThreatLevel += OccupationSquares[k][i]->GetThreatLevel();
		}
		ThreatLevelPerColumn[i] = TempThreatLevel;
		TempThreatLevel = 0.0f;
	}
}

void ADDTheKing::ResolveDowngradeFunctions()
{
	for (std::function<void()> Func : DowngradeFunctions) {
		Func();
	}

	DowngradeFunctions.Empty();
}

void ADDTheKing::AddEnemiesToAlwaysAdd()
{
	for (EDDEnemyIDs EnemyID : EnemiesToAlwaysAdd) {
		CalculatedWaveInfo.EnemiesToSpawn.Add(EnemyEncyclopediaRef[EnemyID]);
	}
}

void ADDTheKing::ResetVariables() 
{
	CurrentStressLevel = 0.0f;
	ResetTempCastleStressBucketVariables();
	CastleStressBucketLevel = 0.0f;

	SpecialWaveNumber = FMath::RandRange(LowerStartingSpecialWaveRandomizer, HigherStartingSpecialWaveRandomizer);
	CurrentWildCardWave = WildCardAppearanceWave;
	GoldenEnemyChance = OriginalGoldenEnemyChance;

	bIsDisrupterHere = false;
	IsSpecialWave = false;
}

void ADDTheKing::OnStressMeterChange(EDDStressMeterTypes StressType)
{
	int32 CurrentWave = EnemySpawner->GetCurrentWave();
	int32 StarterWaveThreshold = EnemySpawner->GetStarterWaveThreshold();

	//Dont do anything with stress meter if The King isnt in charge!!!!!
	if (CurrentWave < StarterWaveThreshold || GameMode->GetPostWaveState())
		return;

	float StressRate = 0.0f;
	float StressRateMultiplier = 1.0f;
	float DifficultyStrategicSpawnChance;
	EDifficulty Difficulty = GameMode->GetDifficulty();

	switch (Difficulty) {
		case EDifficulty::Easy:
			StressRateMultiplier = EasyStressRateMultiplier;
			DifficultyStrategicSpawnChance = EasyStrategicSpawnChance;
			break;
		case EDifficulty::Normal:
		default:
			DifficultyStrategicSpawnChance = NormalStrategicSpawnChance;
			break;
		case EDifficulty::Hard:
			StressRateMultiplier = HardStressRateMultiplier;
			DifficultyStrategicSpawnChance = HardStrategicSpawnChance;
			break;
	}

	switch (StressType) {
		case EDDStressMeterTypes::EnemySpawn:
			StressRate = CurrentStressLevel + (EnemySpawnStressRate * StressRateMultiplier);
			break;
		case EDDStressMeterTypes::EnemyDeath:
			StressRate = CurrentStressLevel + (EnemyDeathStressRate * StressRateMultiplier);
			break;
		case EDDStressMeterTypes::CastleAttacked:
			//TODO - consider castle being damaged instead of being attacked, since enemies attacking the castle can attack units in front of them
			StressRate = CurrentStressLevel + (CastleAttackedStressRate * StressRateMultiplier);
			CastleStressBucketLevel = FMath::Min(CastleStressBucketLevel + (CastleAttackedStressRate * StressRateMultiplier), ActualMaxStressMeterThreshold);
			ResetTempCastleStressBucketVariables();
			break;
		case EDDStressMeterTypes::PlaceableDeath:
			StressRate = CurrentStressLevel + (PlaceableDeathStressRate * StressRateMultiplier);
			break;
		case EDDStressMeterTypes::PlaceableSpawn:
			StressRate = CurrentStressLevel + (PlaceableSpawnStressRate * StressRateMultiplier);
			break;
		case EDDStressMeterTypes::PlayerUpgrade:
			StressRate = CurrentStressLevel + (PlayerUpgradeStressRate * StressRateMultiplier);
			break;
	}

	CurrentStressLevel = FMathf::Clamp(StressRate, ActualMinStressMeterThreshold, ActualMaxStressMeterThreshold);
	bHasStressLevelChanged = true;

	OnStressChange.Broadcast(CurrentStressLevel, ActualMaxStressMeterThreshold);

	const float CalculatedLowSpawnBase = FMath::Pow(e, LowSpawnIntervalBaseExponent - LowSpawnIntervalBaseDecreaseRate * (CurrentWave - LowSpawnIntervalBaseXOffset));
	const float CalculatedHighSpawnBase = FMath::Pow(e, HighSpawnIntervalBaseExponent - HighSpawnIntervalBaseDecreaseRate * (CurrentWave - HighSpawnIntervalBaseXOffset));
	//Stress Corrected variables use to increase or decrease the calculated spawn base, depending on the stress level
	const float StressCorrectedLowSpawnInterval = FMath::Pow(e, LowSpawnStressCorrectedExponent * CurrentStressLevel) - LowSpawnStressCorrectedBase;
	const float StressCorrectedHighSpawnInterval = FMath::Pow(e, HighSpawnStressCorrectedExponent * CurrentStressLevel) - HighSpawnStressCorrectedBase;

	const float ActualLowSpawnInterval = CalculatedLowSpawnBase + StressCorrectedLowSpawnInterval;
	const float ActualHighSpawnInterval = CalculatedHighSpawnBase + StressCorrectedHighSpawnInterval;

	EnemySpawner->SetHighSpawnInterval(FMath::Max(ActualHighSpawnInterval, HighSpawnIntervalClamp));
	EnemySpawner->SetLowSpawnInterval(FMath::Max(ActualLowSpawnInterval, LowSpawnIntervalClamp));

	//Only do this strategic spawning after an enemy spawn
	//In other words, do potential strategic spawning for the next enemy spawn
	if (StressType == EDDStressMeterTypes::EnemySpawn) {
		const int32 CurrentPlaceableAmount = PlaceableManager->GetPlaceablePool().Num();
		const int32 BeginningPlaceableAmount = PlaceableManager->GetBeginningPlaceableAmount();
		const int32 BeginningSoulAmount = GameMode->GetBeginningSouls();
		const int32 CurrentSoulAmount = GameMode->GetSouls();
		
		float PlaceableRatio = BeginningPlaceableAmount <= 0 ? 0.0f : (float) CurrentPlaceableAmount / BeginningPlaceableAmount;
		float SoulRatio = BeginningSoulAmount <= 0 ? 0.0f : (float) CurrentSoulAmount / BeginningSoulAmount;

		int32 IntPlaceableRatio = (int32) PlaceableRatio;
		float DecimalOnlyPlaceableRatio = PlaceableRatio - IntPlaceableRatio;

		int32 IntSoulRatio = (int32) SoulRatio;
		float DecimalOnlySoulRatio = SoulRatio - IntSoulRatio;

		float PlaceableStrategicIncrease = FMathf::Lerp(0.0f, PlaceableAmountStrategicSpawnMax, DecimalOnlyPlaceableRatio);
		float SoulStrategicIncrease = FMathf::Lerp(0.0f, SoulAmountStrategicSpawnMax, DecimalOnlySoulRatio);

		float StressMeterStrategicReducer = FMathf::Lerp(0.0f, StressMeterStrategicSpawnMax, CurrentStressLevel / ActualMaxStressMeterThreshold);
		//TODO - not the best way to deal with the overflow, maybe do some kind of function to deal with this
		IntPlaceableRatio = FMathf::Clamp(IntPlaceableRatio, 0.0f, PlaceableRatioOverflowMax);
		IntSoulRatio = FMathf::Clamp(IntSoulRatio, 0.0f, SoulRatioOverflowMax);

		float TotalStrategicSpawnChance = (DifficultyStrategicSpawnChance + 
										IntPlaceableRatio + 
										IntSoulRatio + 
										PlaceableStrategicIncrease + 
										SoulStrategicIncrease) - StressMeterStrategicReducer;

		float RandomNumber = FMath::RandRange(0.0f, 100.0f);

		if (RandomNumber <= TotalStrategicSpawnChance) {
			EnemySpawner->SetStrategicSpawnFlag();
		}
	}
}

void ADDTheKing::OnSquareDetectEnemyDeath(FVector2D SquareCoordinate, AEnemy* Enemy)
{
	int32 CurrentWave = EnemySpawner->GetCurrentWave();
	int32 StarterWaveThreshold = EnemySpawner->GetStarterWaveThreshold();

	//Dont do anything with stress meter if The King isnt in charge!!!!!
	//Or if the enemy is a horizontal enemy, we don't account for these guys
	if (CurrentWave < StarterWaveThreshold || GameMode->GetPostWaveState() || Enemy->IsA<ADDHorizontalEnemy>())
		return;

	//Ensure an enemy doesn't reduce stress rate more than once if dying touching multiple squares
	if (Enemy && Enemy != LastEnemyDetectedOnDeath.Get()) {
		float StressRateMultiplier = 1.0f;
		EDifficulty Difficulty = GameMode->GetDifficulty();

		switch (Difficulty) {
		case EDifficulty::Easy:
			StressRateMultiplier = EasyStressRateMultiplier;
			break;
		case EDifficulty::Normal:
		default:
			//Nothing
			break;
		case EDifficulty::Hard:
			StressRateMultiplier = HardStressRateMultiplier;
			break;
		}

		//If an enemy dies close to the castle, the player's stress will still be considered high
		float StressLeveler = FMathf::Lerp(0.0f,
										EnemySpawnStressRate * StressRateMultiplier,
										((OccupationSquareAmount.X - 1) - SquareCoordinate.X) / OccupationSquareAmount.X);
		CurrentStressLevel += StressLeveler;

		LastEnemyDetectedOnDeath = MakeWeakObjectPtr(Enemy);
	}
}

void ADDTheKing::OnThreatLevelChange(FVector2D SquareCoordinate, float OldThreatLevel)
{
	float NewThreatLevel = OccupationSquares[SquareCoordinate.X][SquareCoordinate.Y]->GetThreatLevel();
	
	ThreatLevelPerColumn[SquareCoordinate.Y] -= OldThreatLevel;
	ThreatLevelPerColumn[SquareCoordinate.Y] += NewThreatLevel;
}

void ADDTheKing::OnStrategicEnemySpawn(AEnemy* Enemy)
{
	//Don't mess with horizontal enemies
	if (Enemy->IsA<ADDHorizontalEnemy>()) return;

	float OptimalThreatLevel = -1.0f;
	FDDSecretStats EnemySecretStats = Enemy->GetSecretStats();
	std::function<TPair<float, bool>(float, float)> ComparisonFunc;

	FVector StatVector = FVector(EnemySecretStats.Offense,
								EnemySecretStats.Defense,
								EnemySecretStats.Support);

	StatVector.Normalize();
	//Reuse player style here to determine enemy's style for placement purposes
	EDDPlayerStyle Style = DeterminePlayerStyle(StatVector);
	//Fetch the optimal formula for the spawned enemy
	switch (Style) {
		case EDDPlayerStyle::Aggressive:
		case EDDPlayerStyle::Supportive:
			//Finding the min
			ComparisonFunc = [](float a, float b)
				{
					TPair<float, bool> pair = TPair<float, bool>(-1.0f, false);
					pair.Value = a < b || FMath::IsNearlyEqual(a, b);
					if (pair.Value) pair.Key = a;

					return pair;
				};
			OptimalThreatLevel = INFINITY;
			break;
		case EDDPlayerStyle::Defensive:
		case EDDPlayerStyle::Generalist:
		default:
			//Finding the max
			ComparisonFunc = [](float a, float b)
				{
					TPair<float, bool> pair = TPair<float, bool>(-1.0f, false);
					pair.Value = a > b || FMath::IsNearlyEqual(a, b);
					if (pair.Value) pair.Key = a;

					return pair;
				};
			OptimalThreatLevel = -INFINITY;
			break;
	}

	TPair<float, bool> pair;
	float TempThreatLevel = 0;
	
	TArray<int32> MostThreateningColumns;

	//Find the optimal columns to set the enemy's location
	for (int32 i = 0; i < OccupationSquareAmount.Y; i++) {
		TempThreatLevel = ThreatLevelPerColumn[i];
		pair = ComparisonFunc(TempThreatLevel, OptimalThreatLevel);
		if (pair.Value) {
			if (!FMath::IsNearlyEqual(TempThreatLevel, OptimalThreatLevel)) {
				MostThreateningColumns.Empty();
			}
			MostThreateningColumns.Add(i);
			OptimalThreatLevel = pair.Key;
		}
	}

	if (MostThreateningColumns.IsEmpty()) {
		UE_LOG(LogTemp, Warning, TEXT("Unable to find optimal threatening column, skipping strategic spawning..."))
		return;
	}
	
	const int32 RandomColumn = MostThreateningColumns[FMath::RandRange(0, MostThreateningColumns.Num() - 1)];
	FVector2D SpawnRange = OccupationSquares[0][RandomColumn]->GetSideCoordinates();

	const FVector2D SpawnRangeLimit = SpawnRangeLimiterManager->GetCurrentSpawnRangeLimit();

	if (SpawnRange.X < SpawnRangeLimit.X && SpawnRange.Y > SpawnRangeLimit.X) {
		//Just have the range be strictly within the bounds if its completely outside
		SpawnRange = SpawnRangeLimit;
	}
	else {
		SpawnRange.X = SpawnRangeLimiterManager->LimitEnemySpawnRange(SpawnRange.X);
		SpawnRange.Y = SpawnRangeLimiterManager->LimitEnemySpawnRange(SpawnRange.Y);
	}

	UE_LOG(LogTemp, Log, TEXT("%s spawned on square %d"), *Enemy->GetName(), RandomColumn);

	const float YSpawn = FMath::RandRange(SpawnRange.X, SpawnRange.Y);

	Enemy->SetActorLocation(FVector(Enemy->GetActorLocation().X, YSpawn, Enemy->GetActorLocation().Z));
	if (Enemy->IsA<ADDCraterCreator>()) { //Specifically handle CraterCreator, their crater gets bugged otherwise
		ADDCraterCreator* Creator = Cast<ADDCraterCreator>(Enemy);
		Creator->HandleTeleportation();
	}
}

void ADDTheKing::OnPlayerUpgradedStat()
{
	float TotalUpgradeThreatLevel = 0;

	const TMap<EPlayerStats, TArray<UDDUpgradeShopButton*>> ButtonUpgrades = SoulShopWidget->GetUpgradeButtonsByCategory();
	for (TPair <EPlayerStats, TArray<UDDUpgradeShopButton*>> ButtonPair : ButtonUpgrades) {
		UDDUpgradeShopButton* Butt = ButtonPair.Value[0];
		const int32 CurrentIndex = Butt->GetPriceIndex();
		if (CurrentIndex > 0) {
			TotalUpgradeThreatLevel += Butt->GetPrefixThreatLevel(CurrentIndex - 1);
		}
	}
	CurrentTotalUpgradeThreatLevel = TotalUpgradeThreatLevel;
}

void ADDTheKing::GameStartEventFunction()
{
	if (EnemySpawner == nullptr) EnemySpawner = &(GameMode->GetEnemySpawner());
	if (SpawnRangeLimiterManager == nullptr) SpawnRangeLimiterManager = &(GameMode->GetSpawnRangeLimiterManager());
	if (EnemyEncyclopediaRef.IsEmpty()) EnemyEncyclopediaRef = EnemySpawner->GetEnemyEncyclopedia();
	//If either are empty, then they are both for sure empty
	if (BasicEnemyArray.IsEmpty() || SpecialEnemyArray.IsEmpty()) {
		TSubclassOf<AEnemy> CurrentEnemy;
		for (TPair<EDDEnemyIDs, TSubclassOf<AEnemy>> EnemyPair : EnemyEncyclopediaRef) {
			CurrentEnemy = EnemyPair.Value;
			if (EnemyPair.Value.GetDefaultObject()->GetEnemyType() == EDDEnemyType::Basic) {
				BasicEnemyArray.Add(CurrentEnemy);
			}
			else {
				//Do not want to add golden enemy or disrupter, these guys are extremely special!!!
				bool SpecialEnemyFound = false;
				for (EDDEnemyIDs ID : EnemiesNotToAdd) {
					if (EnemyPair.Value.GetDefaultObject()->GetEnemyID() == (int32) ID) {
						SpecialEnemyFound = true;
						break;
					}
				}
				if (!SpecialEnemyFound) {
					if (EnemyPair.Value.GetDefaultObject()->GetDestroysPlaceables()) {
						AttackingSpecialEnemyArray.Add(CurrentEnemy);
					}
					else {
						SpecialEnemyArray.Add(CurrentEnemy);
					}
				}
			}
		}
	}

	if (PlaceableManager == nullptr) PlaceableManager = &(GameMode->GetPlaceableManager());
	if (Player == nullptr) Player = &(GameMode->GetPlayer());
	if (SoulShopWidget == nullptr) SoulShopWidget = &(GameMode->GetSoulShopWidget());
	if (NegativeSoulShopManager == nullptr) NegativeSoulShopManager = &(GameMode->GetNegativeSoulShopManager());

	SoulShopWidget->OnStartWaveFromBigShop.BindUObject(this, &ADDTheKing::OnStartWaveFromBigShop);
	GameMode->GetNegativeSoulShopWidget().OnCloseNegativeShop.AddUniqueDynamic(this, &ADDTheKing::NegativeSoulShopClosedEventFunction);
	Player->OnUpgradedStat.BindUObject(this, &ADDTheKing::OnPlayerUpgradedStat);

	EnemySpawner->OnSpawnEnemy.BindUObject(this, &ADDTheKing::OnStressMeterChange);
	EnemySpawner->OnDespawnEnemy.BindUObject(this, &ADDTheKing::OnStressMeterChange);
	EnemySpawner->OnEnemyAttackingCastle.BindUObject(this, &ADDTheKing::OnStressMeterChange);
	EnemySpawner->OnStrategicEnemySpawn.BindUObject(this, &ADDTheKing::OnStrategicEnemySpawn);
	PlaceableManager->OnPlaceableSpawn.BindUObject(this, &ADDTheKing::OnStressMeterChange);
	PlaceableManager->OnPlaceableDespawn.BindUObject(this, &ADDTheKing::OnStressMeterChange);
	SoulShopWidget->OnPlayerUpgrade.BindUObject(this, &ADDTheKing::OnStressMeterChange);

	OnStartWaveFromBigShop();
}

void ADDTheKing::WaveOverEventFunction()
{
	CurrentStressLevel = 0;
	ResetTempCastleStressBucketVariables();
	CastleStressBucketLevel = 0.0f;
}

void ADDTheKing::NegativeSoulShopClosedEventFunction()
{
	CheckDisrupterAppearance();
	EnemySpawner->SetEnemiesToSpawn(CalculatedWaveInfo.EnemiesToSpawn);
	EnemySpawner->PreSpawnSpecialEventEnemies();
	CalculatedWaveInfo.EnemiesToSpawn.Empty();
	if (bIsDisrupterHere) {
		OnSpecialEnemyComing.Broadcast();
		bIsDisrupterHere = false;
	}
}

void ADDTheKing::OnStartWaveFromBigShop()
{
	int32 CurrentWave = EnemySpawner->GetCurrentWave();
	int32 StarterWaveThreshold = EnemySpawner->GetStarterWaveThreshold();

	//This occurs if EnemySpawner has not been initialized during the King's GameStart function
	//Avoids kicking off The King early
	if (StarterWaveThreshold < 1) StarterWaveThreshold = 2;

	if (CurrentWave >= StarterWaveThreshold) {
		UE_LOG(LogTemp, Log, TEXT("Current Wave: %d"), CurrentWave);
		UE_LOG(LogTemp, Log, TEXT("-----------------"));
		ResetWaveInfo();
		CheckGoldenEnemyAppearance();
		CheckSpecialWave();
		CalculatePlayerStyle();
		CalculateWellnessPoints();
		CalculateEnemyPoolSize();
		DetermineEnemyPool(); 
		CalculateEnemyAmount();
		CheckWildCardAppearance();
		PityPointRewards();

		FetchOccupationSquareStats();
		//CalculateThreatLevelsPerColumn(); no need for this i think...
		InitializeStressMeter();
		ResolveDowngradeFunctions();
		AddEnemiesToAlwaysAdd();

		EnemySpawner->SetEnemiesToSpawn(CalculatedWaveInfo.EnemiesToSpawn);
		EnemySpawner->SetMaxEnemySpawn(CalculatedWaveInfo.NumberOfEnemies);
		EnemySpawner->SetLowSpawnInterval(CalculatedWaveInfo.LowSpawnInterval);
		EnemySpawner->SetHighSpawnInterval(CalculatedWaveInfo.HighSpawnInterval);

		//Ensure that boss-type enemies are pre-spawned
		EnemySpawner->PreSpawnBossEnemies();

		OnFinishedWaveCalculation.Broadcast();
	}
	else {
		EnemySpawner->UsePredefinedWaves(); // edgarstinky... soooooo smelly :)
	}
	EnemySpawner->StartSpawn();
}

void ADDTheKing::GameOverEventFunction()
{
	for (TArray<ADDOccupationSquare*> SquareArray : OccupationSquares) {
		for (ADDOccupationSquare* Square : SquareArray) {
			Square->ResetOccupationSquare();
		}
	}

	ResetVariables();
}

void ADDTheKing::GameWaveJumpChoiceEventFunction() 
{
	const int32 CurrentWave = EnemySpawner->GetCurrentWave();
	//+ 1 gives a small buffer so the player isn't hit with a special or wild wave right off the bat
	const int32 SpecialMultiplier = (CurrentWave + 1) / LowerSpecialWaveRandomizer;
	const int32 WildMultiplier = (CurrentWave + 1) / LowWildCardAddition;

	SpecialWaveNumber = (SpecialMultiplier * LowerSpecialWaveRandomizer) + FMath::RandRange(LowerSpecialWaveRandomizer, HigherSpecialWaveRandomizer);
	CurrentWildCardWave = (WildMultiplier * LowWildCardAddition) + FMath::RandRange(LowWildCardAddition, HighWildCardAddition);
}
