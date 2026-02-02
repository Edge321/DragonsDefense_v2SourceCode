// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "DDAIButton.generated.h"

enum class EPlaceableAI : uint8;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDAIButton : public UButton
{
	GENERATED_BODY()


protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EPlaceableAI PlaceableAI;

};
