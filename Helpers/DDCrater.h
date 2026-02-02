// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDCrater.generated.h"

class UBoxComponent;

DECLARE_DELEGATE_OneParam(FOnLifetimeFinished, ADDCrater*);

UCLASS()
class DRAGONSDEFENSE_V2_API ADDCrater : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADDCrater();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Componenets")
	UStaticMeshComponent* Mesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Componenets")
	UBoxComponent* Collider;

	//How long does this crater last for
	UPROPERTY(EditAnywhere, Category = "Duration")
	int32 WaveLifetime = 1;

public:	

	const FVector GetMeshSize() const;

	FOnLifetimeFinished OnLifetimeFinished;

private:

	void CheckWaveLifeTime();

	UFUNCTION()
	void WaveOverEventFunction();

};
