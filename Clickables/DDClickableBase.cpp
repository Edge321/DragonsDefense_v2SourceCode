// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDClickableBase.h"
#include "Components/BoxComponent.h"
//My classes
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"

// Sets default values
ADDClickableBase::ADDClickableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	ClickableCollider = CreateDefaultSubobject<UBoxComponent>("ClickableCollider");

	RootComponent = Mesh;
	ClickableCollider->SetupAttachment(Mesh);

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->bRenderCustomDepth = true;
	ClickableCollider->SetCollisionObjectType(ECC_PlaceableChannel);
}

void ADDClickableBase::OnDeath()
{
	OnFinishedClicking.ExecuteIfBound(this);
}

void ADDClickableBase::OnPlayerClicked()
{
	if (--ClicksAmount <= 0) {
		FinishClicksEvent();
	}
	else {
		DisableHighlight();
		GetWorldTimerManager().SetTimer(HighlightClickTimer, 
										this, 
										&ADDClickableBase::EnableHighlight, 
										TimeToEnableHighlight, 
										false, 
										TimeToEnableHighlight);
	}
}

void ADDClickableBase::EnableHighlight()
{
	Mesh->SetCustomDepthStencilValue(RedStencil_HighlightDepth);
}

void ADDClickableBase::DisableHighlight()
{
	Mesh->SetCustomDepthStencilValue(0);
}

void ADDClickableBase::FinishClicksEvent()
{
	OnDeath();
}

