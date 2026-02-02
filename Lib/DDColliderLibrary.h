// Copyright (c) 2024, Edge Cope Corp. All rights reserved

#pragma once

#include "CoreMinimal.h"

class UShapeComponent;

/**
 * 
 */
class DRAGONSDEFENSE_V2_API DDColliderLibrary
{

#define ECC_EnemyChannel ECC_GameTraceChannel1
#define ECC_PlaceableChannel ECC_GameTraceChannel2
#define ECC_PreviewChannel ECC_GameTraceChannel3
#define ECC_AttackRadiusChannel ECC_GameTraceChannel4
#define ECC_EnemyRadiusChannel ECC_GameTraceChannel5

#define ENEMY_COLLIDER_TAG "EnemyCollider"

public:

	//Makes the collider ignore the passed in channel
	static void SetCollisionChannelToIgnore(UShapeComponent* Collider, const ECollisionChannel Channel);
	//Makes collider be able to overlap passed in channel
	static void SetCollisionChannelToOverlap(UShapeComponent* Collider, const ECollisionChannel Channel);
};
