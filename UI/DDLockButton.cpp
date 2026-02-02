// Copyright (c) 2025, Edge Cope Corp. All rights reserved


#include "DDLockButton.h"
#include "Components/Image.h"

void UDDLockButton::InitializeButton()
{
	//Buttons are only allowed one child so it should be the 0th by default
	UWidget* Child = GetChildAt(0);
	if (Child->IsA<UImage>()) {
		LockImage = Cast<UImage>(Child);
	}
	else {
		UE_LOG(LogTemp, Fatal, TEXT("%s does not have an image as a child"), *GetName())
	}
}

void UDDLockButton::SetIcon(UTexture2D* Texture)
{
	LockImage->SetBrushFromTexture(Texture);
}

void UDDLockButton::SetIsLocked(const bool IsLocked)
{
	bIsLocked = IsLocked;
}

const bool UDDLockButton::GetIsLocked() const
{
	return bIsLocked;
}
