// Copyright (c) 2025, Edge Cope Corp. All rights reserved


#include "DDKingAnnouncementsWidget.h"
#include "Components/RichTextBlock.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/DDEnemySpawner.h"
#include "DragonsDefense_v2/Lib/DDWidgetBlueprintLibrary.h"

void UDDKingAnnouncementsWidget::InitializeWidget()
{
	InitializeEmotionMaps();

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->OnGameStart.AddDynamic(this, &UDDKingAnnouncementsWidget::GameStartEventFunction);
		GameMode->OnWaveStart.AddDynamic(this, &UDDKingAnnouncementsWidget::WaveStartEventFunction);
		GameMode->OnGameOver.AddDynamic(this, &UDDKingAnnouncementsWidget::GameOverEventFunction);
		GameMode->OnGameRestart.AddDynamic(this, &UDDKingAnnouncementsWidget::GameOverEventFunction);
		GameMode->OnGameWon.AddDynamic(this, &UDDKingAnnouncementsWidget::GameOverEventFunction);
		GameMode->OnGameWaveJumpChoice.AddDynamic(this, &UDDKingAnnouncementsWidget::GameWaveJumpChoiceEventFunction);
	}

	FindRichTextBlock();

	int32 TheKingWave = 50; //Default boss wave number
	int32 TotalEmotions = StaticEnum<EEmotionState>()->NumEnums() - 1;
	if (GameMode) {
		TheKingWave = GameMode->GetTheKingBossWave();
	}

	EmotionChangeWave = (TheKingWave - 1) / TotalEmotions;
	CurrentEmotion = (EEmotionState)0; //Start at the very first emotion
}

const FString UDDKingAnnouncementsWidget::ConvertMessageToRichText(const FMessageAttributes Message)
{
	if (Message.Attribute == ERichTextAttribute::Default) {
		return Message.Announcement;
	}

	FString MessageString = UEnum::GetValueAsString(Message.Attribute); //Returns as "ERichTextAttribute::Attribute"
	FString CorrectedMessageString = MessageString.Mid(MessageString.Find("::") + 2); //+2 is to get rid of the ::
	FString StrengthString = UEnum::GetValueAsString(Message.Strength); //Returns as "EAttributeStrength::Strength"
	FString CorrectedStrengthString = StrengthString.Mid(StrengthString.Find("::") + 2); //+2 is to get rid of the ::

	return "<" + CorrectedMessageString + CorrectedStrengthString + ">" + Message.Announcement + "</>"; //Assembles rich text format
}

void UDDKingAnnouncementsWidget::GameStartEventFunction()
{
	if (!EnemySpawnerRef.IsValid()) {
		ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GameMode) {
			EnemySpawnerRef = &(GameMode->GetEnemySpawner());
		}
	}
}

void UDDKingAnnouncementsWidget::WaveStartEventFunction()
{
	const int32 CurrentWave = EnemySpawnerRef->GetCurrentWave();

	if ((CurrentWave % EmotionChangeWave) == 0 && (int32)CurrentEmotion < StaticEnum<EEmotionState>()->NumEnums()) {
		CurrentEmotion = (EEmotionState)((int32)CurrentEmotion + 1);
	}

	if ((CurrentWave % WaveAnnouncement) == 0) {
		SetVisibility(ESlateVisibility::HitTestInvisible);
		MakeAnnouncement(ConvertMessageToRichText(GetMessageAttribute()));
		GetWorld()->GetTimerManager().SetTimer(TextLifetimeHandler, this, &UDDKingAnnouncementsWidget::HideAnnouncement, TextLifetime, false);
	}
}

void UDDKingAnnouncementsWidget::GameOverEventFunction() 
{
	CurrentEmotion = EEmotionState::Amused;
}

void UDDKingAnnouncementsWidget::GameWaveJumpChoiceEventFunction() 
{
	int32 CurrentWave = 1;
	if (EnemySpawnerRef.IsValid()) {
		CurrentWave = EnemySpawnerRef->GetCurrentWave();
	}
	else {
		CurrentWave = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode())->GetEnemySpawner().GetCurrentWave();
	}
	const int32 EmotionChange = FMath::Clamp(CurrentWave / EmotionChangeWave, 0, StaticEnum<EEmotionState>()->NumEnums() - 1);
	CurrentEmotion = (EEmotionState)EmotionChange;
}

void UDDKingAnnouncementsWidget::InitializeEmotionMaps()
{
	for (FMessageAttributes Message : Announcements) {
		if (!EmotionMessagesMap.Contains(Message.Emotion)) {
			EmotionMessagesMap.Add(Message.Emotion);
		}
		EmotionMessagesMap[Message.Emotion].Add(Message);
	}
}

void UDDKingAnnouncementsWidget::FindRichTextBlock()
{
	TArray<UWidget*> Widgets = UDDWidgetBlueprintLibrary::SearchWidgetChildren(GetRootWidget(), URichTextBlock::StaticClass());
	RichTextBlock = Cast<URichTextBlock>(Widgets[0]); //Should only exist one
}

FMessageAttributes UDDKingAnnouncementsWidget::GetMessageAttribute() const
{
	const int32 TotalMessages = EmotionMessagesMap[CurrentEmotion].Num();
	const int32 RandomMessage = FMath::RandRange(0, TotalMessages - 1);
	return EmotionMessagesMap[CurrentEmotion][RandomMessage];
}

const FString UDDKingAnnouncementsWidget::GetEnumStringWithSpacedUppercases(const ERichTextAttribute RichTextAttribute)
{
	FString FinalString, TempString;
	TArray<FString> Strings;

	FString EnumString = UEnum::GetValueAsString(RichTextAttribute); //Returns as EPlayerStats::Stat
	FString CorrectedEnumString = EnumString.Mid(EnumString.Find("::") + 2); //+2 is to get rid of the ::

	for (int32 i = 0; i < CorrectedEnumString.Len(); i++) {
		TCHAR C = CorrectedEnumString[i];

		if (FChar::IsUpper(C) && i > 0) {
			Strings.Add(TempString);
			TempString.Empty();
			TempString.Append(" ");
		}

		TempString.AppendChar(C);
	}

	if (!TempString.IsEmpty()) {
		Strings.Add(TempString);
	}

	for (FString String : Strings) {
		FinalString.Append(String);
	}

	return FinalString;
}

void UDDKingAnnouncementsWidget::HideAnnouncement()
{
	SetVisibility(ESlateVisibility::Hidden);
}
