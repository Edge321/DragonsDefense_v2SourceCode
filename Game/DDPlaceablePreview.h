// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DDPlaceablePreview.generated.h"

class UBoxComponent;

DECLARE_DELEGATE(FOnColliding);

UCLASS()
class DRAGONSDEFENSE_V2_API ADDPlaceablePreview : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADDPlaceablePreview();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* Collider;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* OptionalRadiusMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Material")
	void SetMaterial(UMaterialInstance* Material);
	UFUNCTION(BlueprintCallable, Category = "Material")
	const bool GetCurrentlyColliding() const;

	void SetMesh(UStaticMesh* NewMesh);
	void SetScale(FVector Scale);
	void SetRadiusSize(const float AttackRadius, const float RadiusMeshDiameter);
	void EnableAttackRadius();
	void DisableAttackRadius();
	void ClearActorsArray();
	void CalculateOffset();


	FOnColliding OnColliding;

private:

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void GameStartEventFunction();

	void RemoveActor(AActor* Actor);
	void AddActor(AActor* Actor);
	const FVector GetMeshSize() const;
	void GetControllerReference();
	void UpdatePreview();


	TArray<AActor*> ActorsColliding;
	//Used for ignoring occupation squares during placeable preview update
	TArray<AActor*> SquaresToIgnore;

	FVector AdjustedOffset = FVector::Zero();

	APlayerController* Controller;
};
