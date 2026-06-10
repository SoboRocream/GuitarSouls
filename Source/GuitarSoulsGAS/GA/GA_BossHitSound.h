// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_BossHitSound.generated.h"

/**
 * 보스 전용 피격 사운드 GA
 * - GA_HitReaction의 이동/입력 차단 태그 간섭 없이 소리만 재생
 * - 몽타주, BlockAbilitiesWithTag, CancelAbilitiesWithTag 없음
 * - Character_Action_HitReaction 이벤트로 트리거
 */
UCLASS()
class GUITARSOULSGAS_API UGA_BossHitSound : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_BossHitSound();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void CancelAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateCancelAbility) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "HitSound")
	TObjectPtr<class USoundBase> HitSound;
};
