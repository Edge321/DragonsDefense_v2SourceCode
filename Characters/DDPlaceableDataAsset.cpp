// Copyright (c) 2024, Edge Cope Corp. All rights reserved


#include "DDPlaceableDataAsset.h"

//These functions are the most repetitive shit ever I know

const FString UDDPlaceableDataAsset::GetDamageFuzzification() const
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

const FString UDDPlaceableDataAsset::GetHealthFuzzification() const
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

const FString UDDPlaceableDataAsset::GetAttackSpeedFuzzification() const
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

const FString UDDPlaceableDataAsset::GetAttackRadiusFuzzificaiton() const
{
    if (!bOverrideAttackRadiusDescription) {
        for (TPairInitializer<float, FString> Pair : FuzzyAttackRadius) {
            if (AttackRadius < Pair.Key) {
                return Pair.Value;
            }
        }
        return MaxFuzzyDescription;
    }
    else {
        return AttackRadiusDescription;
    }
}
