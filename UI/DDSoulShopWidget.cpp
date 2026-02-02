// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDSoulShopWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/BorderSlot.h"
#include "Components/SizeBoxSlot.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/Game/DDStressMeterTypes.h"
#include "DragonsDefense_v2/Game/DDPlaceableManager.h"
#include "DragonsDefense_v2/UI/DDShopButton.h"
#include "DragonsDefense_v2/UI/DDUpgradeShopButton.h"
#include "Framework/Application/NavigationConfig.h"

void UDDSoulShopWidget::SoulShopWidgetInitialization() 
{
    TSharedRef<FNavigationConfig> NavigationConfig = FSlateApplication::Get().GetNavigationConfig();
    NavigationConfig->bTabNavigation = false;
    NavigationConfig->bAnalogNavigation = false;
    NavigationConfig->bKeyNavigation = false;
    ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
        GameMode->OnGameStart.AddDynamic(this, &UDDSoulShopWidget::GameStartEventFunction);
	}
}

TArray<UDDShopButton*> UDDSoulShopWidget::FindAllShopButtons(UCanvasPanel* Canvas)
{
    TArray<UDDShopButton*> ButtonArray;
    RecursiveFindButtons(Canvas, ButtonArray);
    UE_LOG(LogTemp, Log, TEXT("Button amount: %d"), ButtonArray.Num());
    return ButtonArray;
}

void UDDSoulShopWidget::UpdateButtonsOfSameCategory(UDDUpgradeShopButton* Button)
{
    OnPlayerUpgrade.ExecuteIfBound(EDDStressMeterTypes::PlayerUpgrade);

    EPlayerStats Stat = Button->GetStatUpgrade();
    if (UpgradeButtonsByCategory.Find(Stat) != nullptr) {
        TArray<UDDUpgradeShopButton*> ButtonArray = UpgradeButtonsByCategory[Stat];
        for (UDDUpgradeShopButton* TempButton : ButtonArray) {
            if (Button == TempButton) {
                continue;
            }

            TempButton->IncreasePrice();
        }
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Stat not found in button map. Was the map initialized correctly?"))
    }
}

void UDDSoulShopWidget::BroadcastCollapseBigShop() const
{
    OnClickBigShopButton.Broadcast();
}

void UDDSoulShopWidget::ExecuteOnStartWaveFromBigShopEvent() const
{
    OnStartWaveFromBigShop.ExecuteIfBound();
}

void UDDSoulShopWidget::ExecuteOnRequestShowAllPlaceableInfo() const 
{
    OnRequestShowAllPlaceableInfo.ExecuteIfBound();
}

void UDDSoulShopWidget::ExecuteOnRequestHideAllPlaceableInfo() const 
{
    OnRequestHideAllPlaceableInfo.ExecuteIfBound();
}

void UDDSoulShopWidget::SwitchActiveShopButton(UButton* SrcButton, UButton* DestButton)
{
    if (!SrcButton || !DestButton) {
        UE_LOG(LogTemp, Fatal, TEXT("Error! Did not passed in button variables, skipping function..."))
        return;
    }

    FLinearColor ActiveColor = SrcButton->GetBackgroundColor();
    FWidgetTransform ActiveTransform = SrcButton->GetRenderTransform();
    //Not cooking with this code, but whatever gets the job done with no crashes
    UBorderSlot* SrcBorderSlot = Cast<UBorderSlot>(SrcButton->Slot);
    USizeBoxSlot* SrcSizeSlot = Cast<USizeBoxSlot>(SrcButton->Slot);
    FMargin ActivePadding;
    if (SrcBorderSlot) {
        ActivePadding = SrcBorderSlot->GetPadding();
    }
    else {
        ActivePadding = SrcSizeSlot->GetPadding();
    }

    FLinearColor InactiveColor = DestButton->GetBackgroundColor();
    FWidgetTransform InactiveTransform = DestButton->GetRenderTransform();
    UBorderSlot* DestBorderSlot = Cast<UBorderSlot>(DestButton->Slot);
    USizeBoxSlot* DestSizeSlot = Cast<USizeBoxSlot>(DestButton->Slot);
    FMargin InactivePadding;
    if (DestBorderSlot) {
        InactivePadding = DestBorderSlot->GetPadding();
    }
    else {
        InactivePadding = DestSizeSlot->GetPadding();
    }

    SrcButton->SetBackgroundColor(InactiveColor);
    SrcButton->SetRenderTransform(InactiveTransform);
    if (SrcBorderSlot) {
        SrcBorderSlot->SetPadding(InactivePadding);
    }
    else {
        SrcSizeSlot->SetPadding(InactivePadding);
    }

    DestButton->SetBackgroundColor(ActiveColor);
    DestButton->SetRenderTransform(ActiveTransform);
    if (DestBorderSlot) {
        DestBorderSlot->SetPadding(ActivePadding);
    }
    else if (DestSizeSlot) {
        DestSizeSlot->SetPadding(ActivePadding);
    }
}

void UDDSoulShopWidget::SwitchActiveShopButtonWithoutSrc(UButton* DestButton)
{
    if (!ActiveButton.IsValid()) {
        UE_LOG(LogTemp, Error, TEXT("Failed to set active button before switching shop buttons, skipping function..."))
        return;
    }

    SwitchActiveShopButton(ActiveButton.Get(), DestButton);

    ActiveButton = MakeWeakObjectPtr(DestButton);
}

void UDDSoulShopWidget::StoreActiveShopButton(UButton* Button)
{
    if (Button) {
        ActiveButton = MakeWeakObjectPtr(Button);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Passed in null button for active button, skipping function"))
    }
}

void UDDSoulShopWidget::SwitchActiveShopTab(UWidget* DestWidget)
{
    if (!ActiveBigScrollBox.IsValid()) {
        UE_LOG(LogTemp, Error, TEXT("Failed to set active button before switching shop buttons, skipping function..."))
        return;
    }

    ActiveBigScrollBox->SetVisibility(ESlateVisibility::Collapsed);
    DestWidget->SetVisibility(ESlateVisibility::Visible);
    ActiveBigScrollBox = MakeWeakObjectPtr(DestWidget);
}

void UDDSoulShopWidget::StoreActiveShopTab(UWidget* Widget)
{
    if (Widget) {
        ActiveBigScrollBox = MakeWeakObjectPtr(Widget);
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Passed in null button for active big scroll box, skipping function"))
    }
}

const TArray<FDDButtonStruct> UDDSoulShopWidget::GetUpgradeButtonsArray()
{
    TArray<FDDButtonStruct> ButtonArray;

    for (TPair<EPlayerStats, TArray<UDDUpgradeShopButton*>> Pair : UpgradeButtonsByCategory) {
        FDDButtonStruct ButtonStruct = {
            Pair.Key,
            Pair.Value[0]->GetPriceIndex(),
            Pair.Value[0]->GetPricesSize()
        };
        ButtonArray.Add(ButtonStruct);
    }

    return ButtonArray;
}


const TMap<EPlayerStats, TArray<UDDUpgradeShopButton*>> UDDSoulShopWidget::GetUpgradeButtonsByCategory() const
{
    return UpgradeButtonsByCategory;
}

void UDDSoulShopWidget::AddToButtonMap(UDDUpgradeShopButton* Button)
{
    EPlayerStats Stat = Button->GetStatUpgrade();
    if (UpgradeButtonsByCategory.Find(Stat) == nullptr) {
        UpgradeButtonsByCategory.Add(Stat);
        UpgradeButtonsByCategory[Stat].Add(Button);
    }
    else {
        UpgradeButtonsByCategory[Stat].Add(Button);
    }
}

//chatgpt the goat for this one ngl
void UDDSoulShopWidget::RecursiveFindButtons(UWidget* Widget, TArray<UDDShopButton*>& ButtonArray)
{    
    //Future me, looking back, this function doesn't take into account shop buttons within shop buttons
    //Which makes sense, otherwise buttons could be conflicting with each other
    if (Widget->IsA<UDDShopButton>()) { 
        UDDShopButton* Button = Cast<UDDShopButton>(Widget);
        ButtonArray.Add(Button);
        if (Button->IsA<UDDUpgradeShopButton>()) {
            UDDUpgradeShopButton* UpButton = Cast<UDDUpgradeShopButton>(Button);
            AddToButtonMap(UpButton);
        }
    }
    else if (Widget->IsA<UPanelWidget>()) {
        UPanelWidget* Panel = Cast<UPanelWidget>(Widget);
        TArray<UWidget*> ChildWidgets = Panel->GetAllChildren();
        for (UWidget* ChildWidget : ChildWidgets) {
            //Future me, I say this as its a rare function
            //Recursion in this day and age? How shocking!
            RecursiveFindButtons(ChildWidget, ButtonArray);
        }
    }
}

void UDDSoulShopWidget::GameStartEventFunction() 
{
    ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
    if (GameMode) {
        GameMode->GetPlaceableManager().OnSetPlaceableInfoVisibility.BindUObject(this, &UDDSoulShopWidget::OnSetPlaceableInfoVisibilityEventFunction);
    }
}

