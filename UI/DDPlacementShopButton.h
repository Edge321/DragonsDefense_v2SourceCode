// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/UI/DDInfiniteShopButton.h"
#include "DDPlacementShopButton.generated.h"

class ADDPlaceable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickPlacementButton);

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDPlacementShopButton : public UDDInfiniteShopButton
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Placeable")
	TSubclassOf<ADDPlaceable> PlaceableClass;

	void InitializeButton() override;
	void SetPreviewAndPrice() const;
	void OnClickEventFunction() override;
	void OnHoveredEventFunction() override;
	//Set placement mode
	void PlacementMode(bool IsPlacing) const;

public:

	UPROPERTY(BlueprintAssignable)
	FOnClickPlacementButton OnClickPlacementButton;

};
