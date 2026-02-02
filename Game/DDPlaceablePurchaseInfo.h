// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"

class ADDGameModeBase;

struct FDDPlaceablePurchaseInfo 
{

public:

	bool IsBuyable() const;
	void UpdateSouls();
	void SetPrice(const int32 Price);
	const int32 GetPrice() const;
	void SetGameMode(ADDGameModeBase* Mode);
		
private:

	int32 CurrentPrice = 1;
	ADDGameModeBase* GameMode = nullptr;

};
