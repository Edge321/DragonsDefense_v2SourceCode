// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDClickableBase.generated.h"

#define RedStencil_HighlightDepth 2

class UBoxComponent;

DECLARE_DELEGATE_OneParam(FOnFinishedClicking, ADDClickableBase*);

UCLASS()
class DRAGONSDEFENSE_V2_API ADDClickableBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADDClickableBase();


protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* ClickableCollider;

	UPROPERTY(EditDefaultsOnly, Category = "Stats | Clicks")
	float TimeToEnableHighlight = 0.2f;
	UPROPERTY(EditAnywhere, Category = "Stats | Clicks")
	int32 ClicksAmount = 3;
	
	void OnDeath();

	void virtual FinishClicksEvent();

public:

	FOnFinishedClicking OnFinishedClicking;

	void OnPlayerClicked();
	void EnableHighlight();
	void DisableHighlight();

private:

	FTimerHandle HighlightClickTimer;

};
