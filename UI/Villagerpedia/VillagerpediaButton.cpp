// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "VillagerpediaButton.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
//My classes
#include "DragonsDefense_v2/Characters/Enemy.h"
#include "DragonsDefense_v2/Characters/DDEnemyDataAsset.h"
#include "DragonsDefense_v2/Game/DDGameModeBase.h"

void UVillagerpediaButton::InitializeButton()
{
	OnClicked.AddDynamic(this, &UVillagerpediaButton::SetPageInfo);	
	OnHovered.AddDynamic(this, &UVillagerpediaButton::CheckNewVillagerFlag);

	ImageComponent = FindButtonComponent<UImage>(this);
	check(ImageComponent);
	TextComponent = FindButtonComponent<UTextBlock>(this);
	check(TextComponent);

	OriginalColor = GetStyle().Normal.TintColor.GetSpecifiedColor();

	if (IsKnownByDefault) {
		EnableButton();
	}
	else {
		DisableButton();
	}
}

TSubclassOf<AEnemy> UVillagerpediaButton::GetEnemyClass() const
{
	return EnemyClass;
}

UTexture2D* UVillagerpediaButton::GetEnemyImage() const
{
	return EnemyImage;
}

const int32 UVillagerpediaButton::GetEnemyID() const
{
	UDDEnemyDataAsset* Data = EnemyClass.GetDefaultObject()->EnemyData;
	if (Data) {
		return Data->EnemyID;
	}
	else {
		return -1;
	}
}

void UVillagerpediaButton::EnableButton()
{
	ImageComponent->SetBrushFromTexture(EnemyImage);
	FText Text = FText::FromString(EnemyClass.GetDefaultObject()->EnemyData->EnemyName);
	TextComponent->SetText(Text);
	SetIsEnabled(true);
}

void UVillagerpediaButton::DisableButton()
{
	ImageComponent->SetBrushFromTexture(UnknownImage);
	TextComponent->SetText(FText::FromString(UnknownName));
	SetIsEnabled(false);
}

void UVillagerpediaButton::EnableNewVillagerFlag()
{
	NewVillagerButton = true;
	FButtonStyle BStyle = GetStyle();
	BStyle.Normal.TintColor = FSlateColor(NewVillagerColor);
	SetStyle(BStyle);
}

void UVillagerpediaButton::ChangeToUnknown()
{
	ImageComponent->SetBrushFromTexture(UnknownImage);
}

void UVillagerpediaButton::ChangeToKnown()
{
	ImageComponent->SetBrushFromTexture(EnemyImage);
}

const bool UVillagerpediaButton::GetIsKnownByDefault() const
{
	return IsKnownByDefault;
}

void UVillagerpediaButton::SetPageInfo()
{
	//Set page info stuff here...
	OnSetVillagerPageInfo.ExecuteIfBound(this);
}

void UVillagerpediaButton::CheckNewVillagerFlag()
{
	if (NewVillagerButton) {
		NewVillagerButton = false;
		FButtonStyle BStyle = GetStyle();
		BStyle.Normal.TintColor = FSlateColor(OriginalColor);
		SetStyle(BStyle);
	}
}

//Shout out to chatgpt, real one for not making me code recursion
//Future me, this shit is not hard idk what I thinking back then
template<class T>
T* UVillagerpediaButton::FindButtonComponent(UWidget* RootWidget)
{
	if (RootWidget == nullptr) {
		return nullptr;
	}
	if (RootWidget->IsA<T>()) {
		return Cast<T>(RootWidget);
	}
	
	UPanelWidget* PanelWidget = Cast<UPanelWidget>(RootWidget);
	if (PanelWidget) {
		TArray<UWidget*> Children = PanelWidget->GetAllChildren();
		for (UWidget* Widget : Children) {
			T* FoundWidget = FindButtonComponent<T>(Widget);
			if (FoundWidget) {
				return FoundWidget;
			}
		}
	}

	return nullptr;
}
