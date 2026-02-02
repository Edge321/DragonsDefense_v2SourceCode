// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDFireRuneTrap.h"

void ADDFireRuneTrap::EnableDebuffArea()
{
	if (!bIsOnCooldown) {
		if (DebuffAreaRef.IsValid()) {
			bIsOnCooldown = true;
			DebuffAreaRef->EnableDebuffArea();
			GetWorldTimerManager().SetTimer(ActiveTimer, this, &ADDFireRuneTrap::DisableDebuffArea, FireActive);
			GetWorldTimerManager().SetTimer(CooldownTimer, this, &ADDFireRuneTrap::CheckAfterCooldown, FireCooldown);
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("%s does not have a valid pointer to DebuffArea"), *GetName())
		}
	}
}

void ADDFireRuneTrap::DisableDebuffArea()
{
	if (DebuffAreaRef.IsValid()) {
		DebuffAreaRef->DisableDebuffArea();
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("%s does not have a valid pointer to DebuffArea"), *GetName())
	}
}

void ADDFireRuneTrap::CheckAfterCooldown()
{
	bIsOnCooldown = false;
	if (EnemiesInArea.IsEmpty()) {
		DisableDebuffArea();
	}
	else {
		EnableDebuffArea();
	}
}
