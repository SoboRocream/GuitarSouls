// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_BossPhaseTransition.generated.h"

/**
 * 보스 페이즈 전환 어빌리티
 *
 * 흐름:
 *   무적 + PhaseTransition 태그 부여
 *   → GameplayCue 실행 (선택)
 *   → 전환 몽타주 재생 (미설정 시 WaitDelay로 대체)
 *   → Boss.Phase.2 태그 부여
 *   → 무적 + PhaseTransition 태그 제거
 *   → EndAbility
 *
 * ActivationBlockedTags에 Boss.Phase.2가 등록되어 있어
 * 페이즈 전환 완료 후에는 재발동 불가 (2차 방어)
 */
UCLASS()
class GUITARSOULSGAS_API UGA_BossPhaseTransition : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_BossPhaseTransition();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateCancelAbility) override;

protected:
	// 전환 연출 몽타주 (미설정 시 FallbackDelay로 대체)
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Phase")
	TObjectPtr<UAnimMontage> TransitionMontage;

	// 몽타주 미설정 시 사용할 대기 시간 (초)
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Phase", meta = (ClampMin = "0.0"))
	float FallbackDelay = 2.f;

	// 전환 시 실행할 GameplayCue 태그 (미설정 시 스킵)
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Phase")
	FGameplayTag TransitionCueTag;

private:
	UFUNCTION()
	void OnMontageEnd();

	UFUNCTION()
	void OnMontageInterrupted();

	// GA 강제 취소 후 AnimInstance 직접 재생이 끝났을 때 호출
	UFUNCTION()
	void OnMontageEndedDirect(UAnimMontage* Montage, bool bInterrupted);

	// ASC 태그 처리 + (GA 활성 상태이면) EndAbility. 중복 호출 방지됨
	void FinishTransition();

	bool bTransitionFinished = false;
};
