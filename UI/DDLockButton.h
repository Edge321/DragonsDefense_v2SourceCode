// Copyright (c) 2025, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "DDLockButton.generated.h"

class UImage;
class UTexture2D;

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UDDLockButton : public UButton
{
	GENERATED_BODY()
	

public:
	
	void InitializeButton();

	void SetIcon(UTexture2D* Texture);

	void SetIsLocked(const bool IsLocked);

	const bool GetIsLocked() const;

private:

	UImage* LockImage;

	bool bIsLocked = true;

};
