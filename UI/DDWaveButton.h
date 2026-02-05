// Copyright (c) 2026, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "DDWaveButton.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDWaveButton : public UButton
{
	GENERATED_BODY()

protected:

	//What wave will this button jump to?
	UPROPERTY(EditAnywhere)
	int32 Wave;

public:

	UFUNCTION(BlueprintCallable)
	int32 GetWave() const;
	UFUNCTION(BlueprintCallable)
	void SetWave(int32 NewWave);

};
