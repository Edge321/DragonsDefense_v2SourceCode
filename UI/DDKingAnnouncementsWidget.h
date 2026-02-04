// Copyright (c) 2025, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDKingAnnouncementsWidget.generated.h"

class URichTextBlock;
class ADDEnemySpawner;

UENUM(BlueprintType)
enum class EEmotionState : uint8 {
	Amused,
	Serious,
	Annoyed,
	Frusturated,
	Angry,
	Wrathful,
	LockedIn
};

//What type of animation is on the text
UENUM(BlueprintType)
enum class ERichTextAttribute : uint8 {
	Default,
	Shaky,
	Rippling
};

UENUM(BlueprintType)
enum class EAttributeStrength : uint8 {
	None, //Reserved for Default attribute
	Light,
	Medium,
	Heavy,
	Extreme
};

USTRUCT(BlueprintType)
struct FMessageAttributes {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Announcement;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ERichTextAttribute Attribute;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAttributeStrength Strength;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEmotionState Emotion;

	FString RichTextAttributeToString() const {
		return UEnum::GetValueAsString(Attribute);
	}
};

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDKingAnnouncementsWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Announcement")
	TArray<FMessageAttributes> Announcements;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Announcement")
	float TextLifetime = 5.0f;
	//When to make an annoucement every N waves
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Announcement")
	int32 WaveAnnouncement = 5;

	UFUNCTION(BlueprintCallable)
	void InitializeWidget();
	UFUNCTION(BlueprintImplementableEvent)
	void MakeAnnouncement(const FString& Message);
	//void MakeAnnouncement(const FMessageAttributes Message);

private:

	UFUNCTION(BlueprintPure)
	const FString ConvertMessageToRichText(const FMessageAttributes Message);

	UFUNCTION()
	void GameStartEventFunction();
	UFUNCTION()
	void WaveStartEventFunction();
	UFUNCTION()
	void GameOverEventFunction();
	UFUNCTION()
	void GameWaveJumpChoiceEventFunction();

	void InitializeEmotionMaps();
	void FindRichTextBlock();
	//Gets a message attribute based on what emotion The King is
	FMessageAttributes GetMessageAttribute() const;
	const FString GetEnumStringWithSpacedUppercases(const ERichTextAttribute RichTextAttribute);
	void HideAnnouncement();

	//I wish I could do it as a UPROPERTY for blueprint access but Unreal doesn't like TArray in TMap for UPROPERTYs
	TMap<EEmotionState, TArray<FMessageAttributes>> EmotionMessagesMap;

	URichTextBlock* RichTextBlock;
	
	//Every N waves The King's emotion will change
	int32 EmotionChangeWave;
	EEmotionState CurrentEmotion;

	FTimerHandle TextLifetimeHandler;

	TWeakObjectPtr<ADDEnemySpawner> EnemySpawnerRef;
};
