// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "VillagerpediaButton.generated.h"

class AEnemy;
class UImage;
class UTextBlock;

DECLARE_DELEGATE_OneParam(FOnSetVillagerPageInfo, UVillagerpediaButton*);

/**
 * 
 */
UCLASS()
class DRAGONSDEFENSE_V2_API UVillagerpediaButton : public UButton
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, Category = "Info")
	TSubclassOf<AEnemy> EnemyClass;
	UPROPERTY(EditAnywhere, Category = "Info")
	UTexture2D* EnemyImage;
	UPROPERTY(EditDefaultsOnly, Category = "Info")
	UTexture2D* UnknownImage;
	UPROPERTY(EditAnywhere, Category = "Info")
	bool IsKnownByDefault = false;
	UPROPERTY(EditAnywhere, Category = "Info")
	FString UnknownName = "Unknown";
	UPROPERTY(EditDefaultsOnly, Category = "Info")
	FLinearColor NewVillagerColor = FLinearColor::Yellow;

public:

	UFUNCTION(BlueprintCallable)
	void InitializeButton();
	UFUNCTION(BlueprintPure)
	TSubclassOf<AEnemy> GetEnemyClass() const;
	UFUNCTION(BlueprintPure)
	UTexture2D* GetEnemyImage() const;
	UFUNCTION(BlueprintPure)
	const int32 GetEnemyID() const;
	//Sets image to enemy image and makes button clickable
	UFUNCTION(BlueprintCallable)
	void EnableButton();
	//Sets image to unknown image and makes button unclickable
	UFUNCTION(BlueprintCallable)
	void DisableButton();
	UFUNCTION(BlueprintCallable)
	void EnableNewVillagerFlag();
	//Changes button image to unknown
	UFUNCTION(BlueprintCallable)
	void ChangeToUnknown();
	//Changes button image to currently set enemy
	UFUNCTION(BlueprintCallable)
	void ChangeToKnown();
	UFUNCTION(BlueprintCallable)
	const bool GetIsKnownByDefault() const;

	FOnSetVillagerPageInfo OnSetVillagerPageInfo;

private:

	UFUNCTION()
	void SetPageInfo();
	UFUNCTION()
	void CheckNewVillagerFlag();

	template<class T>
	T* FindButtonComponent(UWidget* RootWidget);

	UImage* ImageComponent;
	UTextBlock* TextComponent;

	FLinearColor OriginalColor;

	bool NewVillagerButton = false;
};
