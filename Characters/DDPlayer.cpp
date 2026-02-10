// Copyright (c) 2026, Edge Cope Corp. All rights reserved

#include "DDPlayer.h"
#include <cmath>
#include "Kismet/GameplayStatics.h"
#include "GameFramework/FloatingPawnMovement.h"
//My classes
#include "DragonsDefense_v2/Game/DDGameModeBase.h"
#include "DragonsDefense_v2/UI/DDNegativeSoulShopWidget.h"
#include "DragonsDefense_v2/UI/DDSoulShopWidget.h"
#include "DragonsDefense_v2/Projectile/DDPlayerProjectile.h"
#include "DragonsDefense_v2/Characters/DDPlaceable.h"
#include "DragonsDefense_v2/Lib/DDColliderLibrary.h"
#include "DragonsDefense_v2/Clickables/DDClickableBase.h"

// Sets default values
ADDPlayer::ADDPlayer()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>("FloatPawnMove");
	LineOfSightMesh = CreateDefaultSubobject<UStaticMeshComponent>("LineOfSight");

	RootComponent = Mesh;

	//Forces collider to be the only one with collision
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LineOfSightMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LineOfSightMesh->SetupAttachment(Mesh);
	Collider->SetupAttachment(Mesh);
	Arrow->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void ADDPlayer::BeginPlay()
{
	Super::BeginPlay();
	
	ValidateProjectile();

	PController = UGameplayStatics::GetPlayerController(this, 0);

	TempHealth = Health;
	MaxHealth = Health;
	ActualMaxHealth = Health;
	ActualDamage = Damage;
	TempDamage = Damage;
	ActualShootSpeed = ShootSpeed;
	TempShootSpeed = ShootSpeed;
	ActualMovementSpeed = MovementSpeed;
	TempMovementSpeed = MovementSpeed;
	ActualProjectileSpeed = ProjectileSpeed;
	TempProjectileSpeed = ProjectileSpeed;
	FloatingPawnMovement->MaxSpeed = MovementSpeed;
	TempCritProjChance = CritProjChance;
	ActualCritProjChance = CritProjChance;
	ActualInaccuracy = Inaccuracy;
	ActualPenetration = Penetration;
	TempPenetration = Penetration;
	ActualProjectileSize = ProjectileSize;
	TempProjectileSize = ProjectileSize;
	ActualLineOfSightLength = LineOfSightLength;
	TempLineOfSightLength = LineOfSightLength;

	OriginalLocation = GetActorLocation();

	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->OnGameOver.AddDynamic(this, &ADDPlayer::GameOverEventFunction);
		GameMode->OnGameStart.AddDynamic(this, &ADDPlayer::GameStartEventFunction);
		GameMode->OnGameRestart.AddDynamic(this, &ADDPlayer::GameRestartEventFunction);
		GameMode->OnGameWon.AddDynamic(this, &ADDPlayer::GameRestartEventFunction);
		GameMode->OnWaveOver.AddDynamic(this, &ADDPlayer::WaveOverEventFunction);
		GameMode->OnGameWaveJumpChoice.AddDynamic(this, &ADDPlayer::WaveOverEventFunction);
		GameMode->OnWaveStart.AddDynamic(this, &ADDPlayer::WaveStartEventFunction);
		GameMode->OnPlacing.AddDynamic(this, &ADDPlayer::PlacementEventFunction);
		GameModeRef = GameMode;
	}
	//Want to disable the input at the beginning obviously!
	GameOverEventFunction();

	UpdateLineOfSight();
}

void ADDPlayer::FindPlaceableWithCursor()
{
	FVector2D MouseScreenPosition;
	FHitResult Hit;

	if (GameModeRef->GetNegativeSoulShopWidget().GetIsNegativeSoulShopActive() ||
		GameModeRef->GetSoulShopWidget().GetIsMouseHoveringWidget()) {
		return;
	}

	if (PController) {
		PController->GetMousePosition(MouseScreenPosition.X, MouseScreenPosition.Y);
	}
	else {
		UE_LOG(LogTemp, Fatal, TEXT("Player controller is null for Player, aborting"))
	}
	
	bool bHit = MouseOnClickable(Hit);

	if (bHit) {
		AActor* Actor = Hit.GetActor();
		if (Actor->IsA<ADDPlaceable>()) {
			ADDPlaceable* Placeable = Cast<ADDPlaceable>(Actor);
			OnClickPlaceable.Broadcast(MouseScreenPosition, Placeable);
		}
		else if (Actor->IsA<ADDClickableBase>()) {
			ADDClickableBase* Click = Cast<ADDClickableBase>(Actor);
			Click->OnPlayerClicked();
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Error: Actor clicked on is NOT a placeable or clickable"))
		}
	}
	else {
		OnClickOutsideOfPlaceable.Broadcast();
	}
}

void ADDPlayer::BroadcastRightClick() const
{
	OnRightClick.Broadcast();
}

void ADDPlayer::Tick(float DeltaTime)
{
	LimitArea();
	CheckMousePosition();
}

// Called to bind functionality to input
void ADDPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ADDPlayer::ResetMods()
{
	MovementSpeedMod = 1;
	DamageMod = 1;
	ShootSpeedMod = 1;
	MaxHealthMod = 1;
	ProjectileSpeedMod = 1;
	CritProjChanceMod = 1;
	PenetrationMod = 1;
	ProjectileSizeMod = 1;
	LineOfSightLengthMod = 1;
	Inaccuracy = 0;
	//Update for the sake of updating the stats in-game
	UpdateMovementSpeed(0);
	UpdateDamage(0);
	UpdateShootSpeed(0);
	UpdateMaxHealth(0);
	UpdateProjectileSpeed(0);
	UpdateCritProjChance(0);
	UpdatePenetration(0);
	UpdateProjectileSize(0);
	UpdateLineOfSightLength(0);
	UpdateLineOfSight();
}

const float ADDPlayer::GetUnModdedMaxHealth() const
{
	return ActualMaxHealth;
}

const float ADDPlayer::GetUnModdedShootSpeed() const
{
	return ActualShootSpeed;
}

const float ADDPlayer::GetUnModdedDamage() const
{
	return ActualDamage;
}

const float ADDPlayer::GetUnModdedProjectileSpeed() const
{
	return ActualProjectileSpeed;
}

const float ADDPlayer::GetUnModdedPenetration() const
{
	return ActualPenetration;
}

const float ADDPlayer::GetUnModdedProjectileSize() const
{
	return ActualProjectileSize;
}

const float ADDPlayer::GetUnModdedLineOfSightLength() const
{
	return ActualLineOfSightLength;
}

const float ADDPlayer::GetHealth() const
{
	return TempHealth;
}

const float ADDPlayer::GetMaxHealth() const
{
	return MaxHealth;
}

const float ADDPlayer::GetMovementSpeed() const
{
	return TempMovementSpeed;
}

const float ADDPlayer::GetShootSpeed() const
{
	return TempShootSpeed;
}

const float ADDPlayer::GetProjectileSpeed() const
{
	return TempProjectileSpeed;
}

const float ADDPlayer::GetCritProjChance() const
{
	return TempCritProjChance;
}

const int32 ADDPlayer::GetPenetration() const
{
	return TempPenetration;
}

const float ADDPlayer::GetProjectileSize() const
{
	return TempProjectileSize;
}

const float ADDPlayer::GetLineOfSightLength() const
{
	return TempLineOfSightLength;
}

const float ADDPlayer::GetDamage() const
{
	return TempDamage;
}

const bool ADDPlayer::IsPlacingState() const
{
	return bIsPlacing;
}

const bool ADDPlayer::IsWaveOver() const
{
	return bIsWaveOver;
}

const FString ADDPlayer::GetPlayerStatFuzzification(const EPlayerStats PlayerStat) const
{
	float Stat = 0;

	switch (PlayerStat) {
		case EPlayerStats::MovementSpeed:
			Stat = TempMovementSpeed;
			break;
		case EPlayerStats::ShootSpeed:
			Stat = TempShootSpeed;
			break;
		case EPlayerStats::MaxHealth:
			Stat = MaxHealth;
			break;
		case EPlayerStats::Health:
			Stat = TempHealth;
			break;
		case EPlayerStats::Damage:
			Stat = TempDamage;
			break;
		case EPlayerStats::ProjectileSpeed:
			Stat = TempProjectileSpeed;
			break;
		case EPlayerStats::Penetration:
			Stat = TempPenetration;
			break;
		case EPlayerStats::CriticalChance:
			Stat = TempCritProjChance;
			break;
		case EPlayerStats::ProjectileSize:
			Stat = TempProjectileSize;
			break;
		case EPlayerStats::LineOfSight:
			Stat = TempLineOfSightLength;
			break;
		case EPlayerStats::Foresight:
			//Foresight not implemented yet
			return "N/A";
			break;
		default:
			Stat = 0;
			UE_LOG(LogTemp, Error, TEXT("Failed to fetch PlayerStats %s"), *PlayerStatStrings[(int32)PlayerStat])
			return "N/A";
	}

	FString FuzzyString = PlayerStatInfo.GetFuzzyStat(PlayerStat, FMath::Abs(Stat));

	return FuzzyString.IsEmpty() ? "N/A" : FuzzyString;
}

void ADDPlayer::UpdateHealth(const float HealthModifier)
{
	TempHealth = FMathf::Clamp(TempHealth + HealthModifier, 0, MaxHealth);

	if (TempHealth <= 0)
	{
		OnDeath();
	}

	OnUpdateHealth.Broadcast(TempHealth, MaxHealth);
}

void ADDPlayer::UpdateMaxHealth(const float MaxHealthModifier)
{
	float LowerBoundMaxHealth = 0.1f;
	ActualMaxHealth = FMathf::Clamp(ActualMaxHealth + MaxHealthModifier, LowerBoundMaxHealth, INFINITY);
	MaxHealth = ActualMaxHealth * MaxHealthMod;
	OnUpdateHealth.Broadcast(TempHealth, MaxHealth);
	OnUpgradedMaxHealth.Broadcast(ActualMaxHealth);
	OnUpgradedStat.ExecuteIfBound();
}

void ADDPlayer::UpdateMovementSpeed(const float MovementSpeedModifier)
{
	float LowerBoundMovement = 1.0f;
	ActualMovementSpeed = FMathf::Clamp(ActualMovementSpeed + MovementSpeedModifier, LowerBoundMovement, INFINITY);
	TempMovementSpeed = ActualMovementSpeed * MovementSpeedMod;
	FloatingPawnMovement->MaxSpeed = TempMovementSpeed;
	OnUpgradedStat.ExecuteIfBound();
}

void ADDPlayer::UpdateShootSpeed(const float ShootSpeedModifier)
{
	//Just an arbitrary number. Prevents shoot speed going down to 0 (basically shooting every frame)
	float MinShootClamp = 0.01f;
	ActualShootSpeed = FMathf::Clamp(ActualShootSpeed + ShootSpeedModifier, MinShootClamp, INFINITY);
	TempShootSpeed = ActualShootSpeed * ShootSpeedMod;
	OnUpgradedShootSpeed.Broadcast(ActualShootSpeed);
	OnUpgradedStat.ExecuteIfBound();
}

void ADDPlayer::UpdateDamage(const float DamageModifier)
{
	//The player gotta do some damage you know
	float MaxDamageClamp = -0.1;
	ActualDamage = FMathf::Clamp(ActualDamage + DamageModifier, -INFINITY, MaxDamageClamp);
	TempDamage = ActualDamage * DamageMod;
	OnUpgradedDamage.Broadcast(ActualDamage);
	OnUpgradedStat.ExecuteIfBound();
}

void ADDPlayer::UpdateProjectileSpeed(const float ProjectileSpeedModifier)
{
	float MinSpeedClamp = 40.0f;
	ActualProjectileSpeed = FMathf::Clamp(ActualProjectileSpeed + ProjectileSpeedModifier, MinSpeedClamp, INFINITY);
	TempProjectileSpeed = ActualProjectileSpeed * ProjectileSpeedMod;
	OnUpgradedProjectileSpeed.Broadcast(ActualProjectileSpeed);
	OnUpgradedStat.ExecuteIfBound();
}

void ADDPlayer::UpdateCritProjChance(const float CritProjChanceModifier)
{
	float MinCritChanceClamp = 0.0f;
	float MaxCritChanceClamp = 100.0f;
	ActualCritProjChance = FMath::Clamp(ActualCritProjChance + CritProjChanceModifier, MinCritChanceClamp, MaxCritChanceClamp);
	TempCritProjChance = ActualCritProjChance * CritProjChanceMod;
	OnUpgradedStat.ExecuteIfBound();
}

void ADDPlayer::UpdatePenetration(const int32 PenetrationModifier)
{
	int32 MinPenetrationClamp = 1;
	ActualPenetration = FMath::Clamp(ActualPenetration + PenetrationModifier, MinPenetrationClamp, INFINITY);
	TempPenetration = ActualPenetration * PenetrationMod;
	OnUpgradedPenetration.Broadcast(ActualPenetration);
	OnUpgradedStat.ExecuteIfBound();
}

void ADDPlayer::UpdateProjectileSize(const float ProjectileSizeModifier)
{
	float MinSizeClamp = 1.0f;
	ActualProjectileSize = FMath::Clamp(ActualProjectileSize + ProjectileSizeModifier, MinSizeClamp, INFINITY);
	TempProjectileSize = ActualProjectileSize * ProjectileSizeMod;
	OnUpgradedProjectileSize.Broadcast(ActualProjectileSize);
	OnUpgradedStat.ExecuteIfBound();
}

void ADDPlayer::UpdateLineOfSightLength(const float LineOfSightModifier)
{
	float MinLengthClamp = 25.0f;
	ActualLineOfSightLength = FMath::Clamp(ActualLineOfSightLength + LineOfSightModifier, MinLengthClamp, INFINITY);
	TempLineOfSightLength = ActualLineOfSightLength * LineOfSightLengthMod;
	UpdateLineOfSight();
	OnUpgradedLineOfSightLength.Broadcast(ActualLineOfSightLength);
	OnUpgradedStat.ExecuteIfBound();
}

void ADDPlayer::SetMovementSpeedMod(const float NewMovementSpeedMod)
{
	MovementSpeedMod = NewMovementSpeedMod;
	UpdateMovementSpeed(0); //Update for the sake of updating
}

void ADDPlayer::SetDamageMod(const float NewDamageMod)
{
	DamageMod = NewDamageMod;
	UpdateDamage(0);
}

void ADDPlayer::SetShootSpeedMod(const float NewShootSpeedMod)
{
	ShootSpeedMod = NewShootSpeedMod;
	UpdateShootSpeed(0);
}

void ADDPlayer::SetMaxHealthMod(const float NewMaxHealthMod)
{
	MaxHealthMod = NewMaxHealthMod;
	UpdateMaxHealth(0);
}

void ADDPlayer::SetInaccuracyMod(const float NewInaccuracyMod)
{
	float TempInaccuracy = FMath::Abs(NewInaccuracyMod);
	Inaccuracy = ActualInaccuracy + TempInaccuracy;
}

void ADDPlayer::SetProjectileSpeedMod(const float NewProjectileSpeedMod)
{
	ProjectileSpeedMod = NewProjectileSpeedMod;
	UpdateProjectileSpeed(0);
}

void ADDPlayer::SetCritProjChanceMod(const float NewCritProjChanceMod)
{
	CritProjChanceMod = NewCritProjChanceMod;
	UpdateCritProjChance(0);
}

void ADDPlayer::SetPenetrationMod(const float NewPenetrationMod)
{
	PenetrationMod = NewPenetrationMod;
	UpdatePenetration(0);
}

void ADDPlayer::SetProjectileSizeMod(const float NewProjectileSizeMod)
{
	ProjectileSizeMod = NewProjectileSizeMod;
	UpdateProjectileSize(0);
}

void ADDPlayer::SetLineOfSightLengthMod(const float NewLineOfSightLengthMod)
{
	LineOfSightLengthMod = NewLineOfSightLengthMod;
	UpdateLineOfSightLength(0);
}

bool ADDPlayer::MouseOnClickable(FHitResult& Hit)
{
	FVector MouseLocation, MouseDirection;

	FVector2D MouseScreenPosition;

	if (PController) {
		PController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);
		PController->GetMousePosition(MouseScreenPosition.X, MouseScreenPosition.Y);
	}
	else {
		UE_LOG(LogTemp, Fatal, TEXT("Player controller is null for Player, aborting"))
	}

	FVector End = (MouseDirection * 10000) + MouseLocation;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_PlaceableChannel));

	TArray<AActor*> ActorsToIgnore;

	//Lots of things happening here, so
	//Basically just returns a FHitResult on the object the line trace collides with first
	bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		MouseLocation,
		End,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		Hit,
		true);

	return bHit;
}

void ADDPlayer::ValidateProjectile()
{
	check(Projectile != nullptr);
}

void ADDPlayer::UpdateLineOfSight()
{
	//Calculating scale on the x-axis using the passed in line of sight
	FVector LineScale = LineOfSightMesh->GetComponentScale();
	FVector LineLocation = LineOfSightMesh->GetRelativeLocation();
	float XLineOfSightSize = LineOfSightMesh->GetStaticMesh()->GetBoundingBox().GetSize().X;
	float XScale = TempLineOfSightLength / XLineOfSightSize;
	LineOfSightMesh->SetRelativeScale3D(FVector(XScale, LineScale.Y, LineScale.Z));

	//Adjusting location correctly after setting scale
	float XNewLocation = ((XLineOfSightSize * XScale)/ 2.0f);
	LineOfSightMesh->SetRelativeLocation(FVector(XNewLocation, LineLocation.Y, LineLocation.Z));
}

void ADDPlayer::ResetStats()
{
	TempHealth = Health;
	ActualMaxHealth = Health;
	ActualMovementSpeed = MovementSpeed;
	ActualShootSpeed = ShootSpeed;
	ActualDamage = Damage;
	ActualProjectileSpeed = ProjectileSpeed;
	ActualCritProjChance = CritProjChance;
	ActualPenetration = Penetration;
	ActualProjectileSize = ProjectileSize;
	ActualLineOfSightLength = LineOfSightLength;

	ResetMods();
}

void ADDPlayer::LimitArea()
{
	FVector NewLocation = GetActorLocation();
	NewLocation.Y = FMath::Clamp(NewLocation.Y, -AreaLimitY, AreaLimitY);
	SetActorLocation(NewLocation);
}

void ADDPlayer::CheckMousePosition()
{
	FHitResult Hit;
	bool bHit = MouseOnClickable(Hit);

	if (bHit) {
		if (bIsPlacing ||
		GameModeRef->GetNegativeSoulShopWidget().GetIsNegativeSoulShopActive() ||
		GameModeRef->GetSoulShopWidget().GetIsMouseHoveringWidget()) {
			return;
		}
		
		AActor* Actor = Hit.GetActor();

		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		PController->SetInputMode(InputMode);

		PController->bShowMouseCursor = true;
		PController->bEnableClickEvents = true;
		PController->bEnableMouseOverEvents = true;

		//TODO - fix this damn cursor bug once and for all, look at Claude for more info
		//FSlateApplication::Get().Clear

		//BUG - cursor does not lose focus of widget after interacting with one, thus not changing cursor
		//need to somehow force update or force losing focus of widget
		PController->CurrentMouseCursor = EMouseCursor::Hand;

		if (CurrentHoverActor == Actor) return;
		
		//Disable actor highlight that was hovered away from
		bIsHoveringPlaceableOrClickable = true;
		CurrentHoverActor = Actor;
		OnHoverOutsideClickable.Broadcast();
		
		//Enable actor highlight that is being hovered over
		if (Actor->IsA<ADDPlaceable>()) {
			ADDPlaceable* Placeable = Cast<ADDPlaceable>(Actor);
			OnHoverPlaceable.Broadcast(Placeable);
		}
		else if (Actor->IsA<ADDClickableBase>()) {
			ADDClickableBase* ClickableBase = Cast<ADDClickableBase>(Actor);
			OnHoverClickable.Broadcast(ClickableBase);
		}
		
	}
	else if (bIsHoveringPlaceableOrClickable) {
		PController->CurrentMouseCursor = EMouseCursor::Default;
		CurrentHoverActor.Reset();
		bIsHoveringPlaceableOrClickable = false;
		OnHoverOutsideClickable.Broadcast();
	}
}

void ADDPlayer::OnDeath()
{
	ADDGameModeBase* GameMode = Cast<ADDGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode) {
		GameMode->GameOver();
	}
}

void ADDPlayer::GameOverEventFunction()
{
	ResetStats();
	APlayerController* PlayController = GetController<APlayerController>();
	DisableInput(PlayController);
	PlayController->bShowMouseCursor = true;
	PlayController->SetInputMode(FInputModeUIOnly());
}

void ADDPlayer::GameStartEventFunction()
{
	APlayerController* PlayController = GetController<APlayerController>();
	EnableInput(PlayController);
	PlayController->SetInputMode(FInputModeGameAndUI());
	PlayController->FlushPressedKeys();
	SetActorLocation(OriginalLocation);
}

void ADDPlayer::GameRestartEventFunction()
{
	GameOverEventFunction();
	SetActorLocation(OriginalLocation);
	bIsWaveOver = false;
}

void ADDPlayer::WaveOverEventFunction()
{
	APlayerController* PlayController = GetController<APlayerController>();
	PlayController->FlushPressedKeys();
	bIsWaveOver = true;
}

void ADDPlayer::WaveStartEventFunction()
{
	SetActorLocation(OriginalLocation);
	bIsWaveOver = false;
}

void ADDPlayer::PlacementEventFunction(bool IsPlacing)
{
	bIsPlacing = IsPlacing;
}
