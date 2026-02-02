// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Components/CanvasPanel.h"
#include "DDWidgetBlueprintLibrary.generated.h"

class UCanvasPanel;
class UPanelWidget;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDWidgetBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	* @brief Switches the menus the player is currently on
	*
	* @param MenuDest Menu the player is going to
	* @param MenuSrc Menu the player is coming from
	*/
	UFUNCTION(BlueprintCallable, Category = "Widget")
	static void SwitchMenus(UCanvasPanel* MenuDest, UCanvasPanel* MenuSrc);
	/**
	* @brief Searches for a specific class in a widget's children recursively, up until the search amount
	*
	* @param Widget Widget to search children recursively
	* @param WidgetClassToFind Widgets to find with this class
	* @param SearchAmount How deep is the search. Leave at 0 to search as deep as possible
	*
	* @return Returns an array of widgets with the specified class
	*/
	UFUNCTION(BlueprintCallable, Category = "Widget")
	static TArray<UWidget*> SearchWidgetChildren(UWidget* Widget, const TSubclassOf<UWidget> WidgetClassToFind, const int32 SearchAmount = 0);
	/**
	* @brief Searches for a specific class in a widget's parent recursively
	* 
	* @param Widget Widget to search parents recursively
	* @param SearchAmount How deep is the search. Leave at 0 to search until finding a nullptr
	* 
	* @return Returns a parent of the specified class or nullptr if not found
	* 
	* @note Instantiated within header file since template classes throw errors if instantiated in a .cpp file
	*/
	template <typename T> static T* SearchWidgetParent(UPanelWidget* Widget, int32 SearchAmount = 0)
	{
		UWidget* ParentWidget = Widget->GetParent();

		int32 CurrentSearchNum = SearchAmount <= 0 ? -INFINITY : 0;

		while (ParentWidget != nullptr && CurrentSearchNum < SearchAmount) {
			if (ParentWidget->IsA<T>()) {
				return Cast<T>(ParentWidget);
			}
			else {
				ParentWidget = ParentWidget->GetParent();
			}

			CurrentSearchNum++;
		}

		return nullptr;
	}

private:

	static void RecursiveSearchWidgetChildren(UWidget* Widget, TArray<UWidget*>& WidgetArray, const TSubclassOf<UWidget> WidgetClassToFind, int32& SearchAmount);

	static const int32 BIG_INT = 9999999;

};


