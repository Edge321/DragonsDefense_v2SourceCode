// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDPlaceableInfoWidget.generated.h"

class ADDPlaceable;
class ADDSentientPlaceable;
class ADDTrapPlaceable;
class ADDPlaceableManager;
class UCanvasPanel;
class UCanvasPanelSlot;

enum class EPlaceableAI : uint8;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDPlaceableInfoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	UFUNCTION(BlueprintCallable, Category = "Placeable")
	void InitializeInfoWidget();
	UFUNCTION(BlueprintCallable, Category = "Placeable")
	void SetCurrentPlaceable(ADDPlaceable* CurrentPlaceable);
	UFUNCTION(BlueprintCallable, Category = "Placeable")
	void SetCurrentHoveredPlaceable(ADDPlaceable* CurrentPlaceable);
	UFUNCTION(BlueprintCallable, Category = "PlaceableAI")
	void ChangeBehavior(const EPlaceableAI AI);
	UFUNCTION(BlueprintCallable, Category = "Widget")
	FVector2D CheckOutOfBounds(const FVector2D PanelSize, const FVector2D CursorLocation) const;
	void GetStats();
	UFUNCTION(BlueprintImplementableEvent, Category = "Placeable")
	void CloseMenuOnPlaceable(ADDPlaceable* Placeable);

	UPROPERTY(BlueprintReadOnly, Category = "Placeable")
	ADDPlaceable* MostRecentPlaceable;
	UPROPERTY(BlueprintReadOnly, Category = "Placeable")
	TWeakObjectPtr<ADDPlaceable> MostRecentHoveredPlaceable;
	UPROPERTY(BlueprintReadOnly, Category = "Placeable|Sentient")
	ADDSentientPlaceable* Sentient;
	UPROPERTY(BlueprintReadOnly, Category = "Placeable|Trap")
	ADDTrapPlaceable* Trap;
	UPROPERTY(BlueprintReadOnly, Category = "Placeable")
	bool bIsSentient;
	UPROPERTY(BlueprintReadOnly, Category = "Placeable|Placeable Manager")
	TWeakObjectPtr<ADDPlaceableManager> PlaceableManager;

private:

	void ShowAllPlaceableInfo();
	void HideAllPlaceableInfo();

};
