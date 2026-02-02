// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/CanvasPanel.h"
#include "DDCollapsibleCanvas.generated.h"

UENUM(BlueprintType)
enum class EPosition : uint8 {
	Left,
	Right,
	Top,
	Bottom
};

class UButton;
class UCanvasPanelSlot;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDCollapsibleCanvas : public UCanvasPanel
{
	GENERATED_BODY()

protected:


	
	UFUNCTION(BlueprintCallable, Category = "CollapseButton")
	void SetButtonPosition();
	UFUNCTION(BlueprintCallable, Category = "Canvas")
	void SetCanvasPosition();
	UFUNCTION(BlueprintCallable, Category = "Canvas")
	void SetOwnCanvasPosition(const FVector2D AnchorsMin, const FVector2D AnchorsMax, const FVector2D CanvasAlignment);
	UFUNCTION(BlueprintCallable, Category = "Canvas")
	void CollapseButtonClickEventFunction();
	UFUNCTION(BlueprintCallable, Category = "Canvas")
	void ForceInstantCollapse();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CollapseButton")
	UButton* CollapseButton;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Position")
	EPosition Position = EPosition::Top;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Position")
	float CollapseAnimationTime = 1.0f;

public:

	UFUNCTION(BlueprintCallable)
	void CanvasTick(float DeltaTime);
	UFUNCTION(BlueprintPure, Category = "Canvas")
	const bool IsCollapsed() const;


private:

	UFUNCTION(BlueprintCallable, Category = "CollapseButton")
	void ValidateButton();

	/// <summary>
	/// Starts up the collapse animation
	/// </summary>
	/// <param name="Start">The original position of the canvas</param>
	/// <param name="End">The desired position of the canvas</param>
	void StartCollapseAnimation(const FVector2D End);

	UCanvasPanelSlot* ButtonCollapseSlot;
	UCanvasPanelSlot* CanvasCollapseSlot;

	FVector2D OriginalAlignment;
	FVector2D StartAlignment;
	FVector2D EndAlignment;

	float TempTime = 0.0f;

	bool bIsCollapsed = false;
	bool bIsCollapsing = false;

};
