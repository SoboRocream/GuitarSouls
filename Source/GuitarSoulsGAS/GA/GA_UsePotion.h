// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GA_UsePotion.generated.h"

/**
 * 포션 사용 어빌리티
 * - PotionCount > 0 확인 → CommitAbility(PotionCount -1) → Health 즉시 회복 → 몽타주 재생
 * - 몽타주 미설정 시 즉시 종료
 */
UCLASS()
class GUITARSOULSGAS_API UGA_UsePotion : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_UsePotion();

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;

	// 회복량 (BP에서 조정 가능)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Potion", meta = (ClampMin = "0.0"))
	float HealAmount = 300.f;

	// 포션 마시는 몽타주 (미설정 시 애니메이션 없이 즉시 적용)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Potion")
	TObjectPtr<UAnimMontage> PotionMontage;

	// 회복 연출 GameplayCue 태그 (미설정 시 Cue 미발동)
	// BP에서 GameplayCue.Character.Heal 등으로 설정
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Potion")
	FGameplayTag HealCueTag;

private:
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();
};
