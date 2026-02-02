// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDColliderLibrary.h"
#include "Components/ShapeComponent.h"

void DDColliderLibrary::SetCollisionChannelToIgnore(UShapeComponent* Collider, const ECollisionChannel Channel)
{
	Collider->SetCollisionResponseToChannel(Channel, ECR_Ignore);
}

void DDColliderLibrary::SetCollisionChannelToOverlap(UShapeComponent* Collider, const ECollisionChannel Channel)
{
	Collider->SetCollisionResponseToChannel(Channel, ECR_Overlap);
}
