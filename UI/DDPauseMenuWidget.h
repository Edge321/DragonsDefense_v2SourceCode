// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DDPauseMenuWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnPauseMenuVillagerpedia, UUserWidget*);

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintCallable)
	void OpenVillagerpedia();

public:

	FOnPauseMenuVillagerpedia OnPauseMenuVillagerpedia;

};
