// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DDPlaceableStateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDDPlaceableStateInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DRAGONSDEFENSE_V2_API IDDPlaceableStateInterface
{
	GENERATED_BODY()

public:

	virtual void OnInitialize(AActor* Owner) PURE_VIRTUAL(IDDPlaceableStateInterface::OnInitialize, );
	virtual void HandleBeginOverlap(AActor* Actor) PURE_VIRTUAL(IDDPlaceableStateInterface::HandleBeginOverlap,);
	virtual void HandleEndOverlap(AActor* Actor) PURE_VIRTUAL(IDDPlaceableStateInterface::HandleEndOverlap, );
	virtual void SetOwner(AActor* Actor) PURE_VIRTUAL(IDDPlaceableStateInterface::SetOwner, );
	virtual void OnUninitialize() PURE_VIRTUAL(IDDPlaceableStateInterface::OnUninitialize, );

protected:

	void PrintWrongActorError(const AActor* Actor) const;
	void PrintNullPtrError() const;

};
