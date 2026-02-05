// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDMainMenuWidget.generated.h"

class UCanvasPanel;
enum class EDifficulty : uint8;

DECLARE_DELEGATE_OneParam(FOnMainMenuVillagerpedia, UUserWidget*);
DECLARE_DELEGATE_OneParam(FOnToggleEnemyAttackBoxes, bool);
//When the player decides to start on a wave that isn't the beginning
DECLARE_DELEGATE(FOnWaveJumpChoice);

UCLASS()
class DRAGONSDEFENSE_V2_API UDDMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Difficulty Menu")
	FString EasyInfoText;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Difficulty Menu")
	FString NormalInfoText;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Difficulty Menu")
	FString HardInfoText;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Difficulty Menu")
	float ActiveColorMultiplier = 0.5f;

	UFUNCTION(BlueprintCallable)
	void InitializeWidget();
	UFUNCTION(BlueprintCallable)
	void Start();
	UFUNCTION(BlueprintCallable)
	void SetGameWave() const;
	UFUNCTION(BlueprintCallable)
	void SetGameDifficulty() const;
	//Set the difficulty variable within the main menu widget class
	UFUNCTION(BlueprintCallable)
	void SetDifficultyChosen(const EDifficulty Difficulty);
	//Set the wave variable within the main menu widget class
	UFUNCTION(BlueprintCallable)
	void SetWaveChosen(const int32 Wave);
	UFUNCTION(BlueprintCallable)
	void Quit() const;
	UFUNCTION(BlueprintCallable)
	void OpenVillagerpedia();
	UFUNCTION(BlueprintCallable)
	void ExecuteToggleEnemyAttackBoxes(const bool AttackBoxVisibility) const;
	UFUNCTION(BlueprintCallable)
	void SetIsWaveJumping(const bool WaveJumping);
	UFUNCTION(BlueprintCallable)
	void SwitchDifficultyButton(UButton* DestButton);
	UFUNCTION(BlueprintCallable)
	void SwitchWaveButton(UButton* DestButton);
	//Removes the active color from the current button chosen
	UFUNCTION(BlueprintCallable)
	void ResetDifficultyButton();
	UFUNCTION(BlueprintCallable)
	void ResetWaveButton();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (ForceAsFunction))
	void OnChosingDifficultyButton();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (ForceAsFunction))
	void OnLeavingDifficultyMenu();
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (ForceAsFunction))
	void EnableWaveButtons(const int32 WaveHighScore);

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<UButton> CurrentWaveButton;
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<UButton> CurrentDifficultyButton;
	UPROPERTY(BlueprintReadOnly)
	EDifficulty CurrentDifficultyChosen;

public:

	FOnMainMenuVillagerpedia OnMainMenuVillagerpedia;
	FOnToggleEnemyAttackBoxes OnToggleEnemyAttackBoxes;
	//When the player decides to start on a wave that isn't the beginning
	FOnWaveJumpChoice OnWaveJumpChoice;

	UFUNCTION(BlueprintImplementableEvent, meta = (ForceAsFunction))
	void LoadUnlockedWaves(const TMap<EDifficulty, int32>& DifficultyWaveHighScores);

private:
	
	UFUNCTION()
	void GameStartEventFunction();

	FLinearColor OriginalDifficultyButtonColor;
	FLinearColor OriginalWaveButtonColor;

	int32 CurrentWaveChosen;
	//Flag for if the player decided to wave jump
	bool bIsWaveJumping = false;

};
