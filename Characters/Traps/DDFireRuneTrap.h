// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "DragonsDefense_v2/Characters/Traps/DDDebuffAreaTrap.h"
#include "DDFireRuneTrap.generated.h"

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API ADDFireRuneTrap : public ADDDebuffAreaTrap
{
	GENERATED_BODY()

protected:

	//How long the fire rune is recharging
	UPROPERTY(EditAnywhere, Category = "Stats|Debuff")
	float FireCooldown = 3.0f;
	//How long the fire rune is active for
	UPROPERTY(EditAnywhere, Category = "Stats|Debuff")
	float FireActive = 1.0f;

	void EnableDebuffArea() override;
	void DisableDebuffArea() override;

private:

	void CheckAfterCooldown();

	bool bIsOnCooldown = false;

	FTimerHandle CooldownTimer;
	FTimerHandle ActiveTimer;

};
