// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDClickableManager.generated.h"

class UBillboardComponent;
class ADDClickableBase;

UCLASS()
class DRAGONSDEFENSE_V2_API ADDClickableManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADDClickableManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* ManagerIcon;

public:

	void SpawnClickable(TSubclassOf<ADDClickableBase> ClickClass, FVector Location = FVector::Zero(), FRotator Rotation = FRotator::ZeroRotator);

private:

	void RemoveClickableFromPool(ADDClickableBase* Clickable);
	void CleanPool();

	UFUNCTION()
	void HoverClickableEventFunction(ADDClickableBase* Clickable);
	UFUNCTION()
	void HoverOutsideClickableFunction();
	UFUNCTION()
	void GameStartEventFunction();
	UFUNCTION()
	void GameOverEventFunction();
	UFUNCTION()
	void WaveOverEventFunction();
	

	TArray<ADDClickableBase*> ClickablesPool;

	TWeakObjectPtr<ADDClickableBase> CurrentHoveredClickable;

};
