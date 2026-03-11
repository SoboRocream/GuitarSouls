// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GSGASWeaponData.generated.h"

/**
 * 공격 타입별 데이터 묶음.
 * AttackDataMap의 Value로 사용.
 * 
 * ex)
 * Character.Attack.Light  → AM_Sword_Light, 스태미나 7, 배율 1.0
 * Character.Attack.Heavy  → AM_Sword_Heavy, 스태미나 12, 배율 1.8
 */
USTRUCT(BlueprintType)
struct FGSGASAttackData
{
	GENERATED_BODY()

	// 해당 공격 타입의 몽타주
	// LightAttack 섹션 구조: Attack_1 / Attack_2 / Attack_3
	// HeavyAttack 섹션 구조: Attack_Heavy
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage;

	// 스태미나 소모량
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StaminaCost = 0.f;

	// 데미지 배율 (BaseDamage에 곱함)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DamageMultiplier = 1.f;
};

/**
 * 무기별 DataAsset.
 * 공격 타입 추가 시 AttackDataMap에 태그 + FGSGASAttackData만 추가하면 됨.
 * 
 * ex) 한손검A / 한손검B — AttackDataMap 공유, BaseDamage만 다른 DataAsset 생성
 */
UCLASS()
class GUITARSOULSGAS_API UGSGASWeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	float BaseDamage = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TMap<FGameplayTag, FGSGASAttackData> AttackDataMap;

public:
	// 태그로 AttackData 조회. 없으면 nullptr 반환.
	const FGSGASAttackData* GetAttackData(const FGameplayTag& AttackTag) const;

	
};
