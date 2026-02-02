// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDEnemyDataAsset.h"

const FString UDDEnemyDataAsset::GetDamageFuzzification() const
{
    if (!bOverrideDamageDescription) {
        for (TPairInitializer<float, FString> Pair : FuzzyDamage) {
            if (FMathf::Abs(Damage) < Pair.Key) {
                return Pair.Value;
            }
        }
        return MaxFuzzyDescription;
    }
    else {
        return DamageDescription;
    }
}

const FString UDDEnemyDataAsset::GetHealthFuzzification() const
{
    if (!bOverrideHealthDescription) {
        for (TPairInitializer<float, FString> Pair : FuzzyHealth) {
            if (Health < Pair.Key) {
                return Pair.Value;
            }
        }
        return MaxFuzzyDescription;
    }
    else {
        return HealthDescription;
    }
}

const FString UDDEnemyDataAsset::GetAttackSpeedFuzzification() const
{
    if (!bOverrideAttackSpeedDescription) {
        for (TPairInitializer<float, FString> Pair : FuzzyAttackSpeed) {
            if (AttackSpeed > Pair.Key) {
                return Pair.Value;
            }
        }
        return MaxFuzzyDescription;
    }
    else {
        return AttackSpeedDescription;
    }
}

const FString UDDEnemyDataAsset::GetMovementSpeedFuzzificaiton() const
{
    if (!bOverrideMovementSpeedDescription) {
        for (TPairInitializer<float, FString> Pair : FuzzyMovementSpeed) {
            if (MovementSpeed < Pair.Key) {
                return Pair.Value;
            }
        }
        return MaxFuzzyDescription;
    }
    else {
        return MovementSpeedDescription;
    }
}

const FString UDDEnemyDataAsset::GetSoulValueFuzzification() const
{
    if (!bOverrideSoulValueDescription) {
        for (TPairInitializer<float, FString> Pair : FuzzySoulValue) {
            if (SoulValue < Pair.Key) {
                return Pair.Value;
            }
        }
        return MaxFuzzyDescription;
    }
    else {
        return SoulValueDescription;
    }
}
