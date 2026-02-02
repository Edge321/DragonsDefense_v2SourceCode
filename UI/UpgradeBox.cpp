// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "UpgradeBox.h"
#include "Components/Image.h"

void UUpgradeBox::InitializeImages()
{
	for (int i = 0; i < UpgradeImagesSize; i++) {
		NewUpgradeImage();
	}
}

void UUpgradeBox::SetUpgradeBox(const int32 CurrentUpgrade, const int32 MaxUpgrades)
{
	int32 Size = FMath::Max(MaxUpgrades, UpgradeArray.Num());
	for (int i = 0; i < Size; i++) {		
		//Allocate more images if we ran out
		if (i >= UpgradeArray.Num()) {
			NewUpgradeImage();
		}
		
		UpgradeArray[i]->SetVisibility(ESlateVisibility::HitTestInvisible);

		if (i >= MaxUpgrades) {
			UpgradeArray[i]->SetVisibility(ESlateVisibility::Collapsed);
		}
		
		if (i < CurrentUpgrade) {
			UpgradeArray[i]->SetBrushFromTexture(UpgradeImage);
		}
		else if (i < MaxUpgrades) {
			UpgradeArray[i]->SetBrushFromTexture(NonUpgradeImage);
		}
	}
}

void UUpgradeBox::NewUpgradeImage()
{
	UImage* Image = NewObject<UImage>(this);
	Image->SetBrushFromTexture(NonUpgradeImage);
	FSlateBrush Brush;
	Brush.SetImageSize(ImageSize);
	Image->SetBrush(Brush);
	AddChildToHorizontalBox(Image);
	UpgradeArray.Add(Image);
}

