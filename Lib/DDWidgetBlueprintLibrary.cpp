// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDWidgetBlueprintLibrary.h"
#include "Components/PanelWidget.h"
//My classes

void UDDWidgetBlueprintLibrary::SwitchMenus(UCanvasPanel* MenuDest, UCanvasPanel* MenuSrc)
{
	if (MenuDest && MenuSrc) {
		if (MenuSrc->GetVisibility() == ESlateVisibility::Hidden) {
			UE_LOG(LogTemp, Warning, TEXT("Warning: Menu source is already hidden!"))
		}
		if (MenuSrc->GetVisibility() == ESlateVisibility::Visible) {
			UE_LOG(LogTemp, Warning, TEXT("Warning: Menu destination is already hidden!"))
		}

		MenuSrc->SetVisibility(ESlateVisibility::Hidden);
		MenuDest->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error: %s is a null pointer. Check if it was assigned correctly"),
			MenuDest ? "Menu Destination" : "Menu Source")
	}
}

TArray<UWidget*> UDDWidgetBlueprintLibrary::SearchWidgetChildren(UWidget* Widget, const TSubclassOf<UWidget> WidgetClassToFind, const int32 SearchAmount)
{
	if (!Widget) return TArray<UWidget*>();

	TArray<UWidget*> WidgetChildren;
	int32 CurrentSearch = SearchAmount <= 0 ? BIG_INT : SearchAmount;

	RecursiveSearchWidgetChildren(Widget, WidgetChildren, WidgetClassToFind, CurrentSearch);

	return WidgetChildren;
}

void UDDWidgetBlueprintLibrary::RecursiveSearchWidgetChildren(UWidget* Widget, TArray<UWidget*>& WidgetArray, const TSubclassOf<UWidget> WidgetClassToFind, int32& SearchAmount)
{
	if (!Widget || SearchAmount <= 0) {
		return;
	}

	if (Widget->IsA(WidgetClassToFind)) {
		WidgetArray.Add(Widget);
	}
	
	UPanelWidget* PanelWidget = Cast<UPanelWidget>(Widget);

	if (PanelWidget) {
		TArray<UWidget*> WidgetChildren = PanelWidget->GetAllChildren();

		//SearchAmount--; TODO - implement search amount
		for (UWidget* WidgetChild : WidgetChildren) {
			RecursiveSearchWidgetChildren(WidgetChild, WidgetArray, WidgetClassToFind, SearchAmount);
		}
	}
}
