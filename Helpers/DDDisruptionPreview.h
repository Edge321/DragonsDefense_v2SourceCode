// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDDisruptionPreview.generated.h"

UCLASS()
class DRAGONSDEFENSE_V2_API ADDDisruptionPreview : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADDDisruptionPreview();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Mesh;

	//How fast the material does the blinking effect
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	float OpacityChange = 0.1f;
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	float MaxOpacity = 0.8f;
	UPROPERTY(EditDefaultsOnly, Category = "Materials")
	float MinOpacity = 0.0f;

	UFUNCTION(BlueprintImplementableEvent)
	void AnimateMaterialColor();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UFUNCTION(BlueprintCallable)
	void DisablePreview();
	UFUNCTION(BlueprintCallable)
	void EnablePreview();

	void EnableBlinkingEffect();
	void DisableBlinkingEffect();
	void FetchDisruptionTransform();
	void UpdateOpacity();

	UFUNCTION()
	void GameStartEventFunction();
	UFUNCTION()
	void GameOverEventFunction();
	UFUNCTION()
	void WaveStartEventFunction();
	UFUNCTION()
	void SpecialEnemyIncomingEventFunction();

	bool bIsSpecialEnemyComing = false;

	//Variables for blinking material effect
	FTimerHandle BlinkingTimer;
	bool bIsIncreasing = true;
	float CurrentOpacity = 0.8f;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

};
