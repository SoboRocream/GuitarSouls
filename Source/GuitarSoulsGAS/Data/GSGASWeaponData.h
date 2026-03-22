// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GSGASWeaponData.generated.h"

/**
 
 */
UCLASS()
class GUITARSOULSGAS_API UGSGASWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float BaseDamage = 15.f;
 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TMap<FGameplayTag, float> StaminaCostMap;
 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TMap<FGameplayTag, float> DamageMultiplierMap;
 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TMap<FGameplayTag, TObjectPtr<UAnimMontage>> MontageMap;

public:
	// 태그로 몽타주 조회. 없으면 nullptr 반환.
	UAnimMontage* GetMontageForTag(const FGameplayTag& Tag) const;
 
	// 태그로 스태미나 소모량 조회. 없으면 0 반환.
	float GetStaminaCost(const FGameplayTag& Tag) const;
 
	// 태그로 최종 데미지 조회 (BaseDamage * Multiplier). Multiplier 없으면 BaseDamage 반환.
	float GetFinalDamage(const FGameplayTag& Tag) const;
	
};
