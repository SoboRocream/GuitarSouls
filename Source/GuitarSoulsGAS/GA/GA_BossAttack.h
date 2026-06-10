// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_BossAttack.generated.h"

/**
 * 보스 공격 어빌리티
 *
 * GA_EnemyAttack 기반, 스태미나/콤보 제거, 몽타주 GA 직접 지정
 * - 무기 충돌 + 데미지 파이프라인은 동일
 * - 각 공격 패턴은 BP 자식 클래스에서 AttackMontage만 교체
 * - 타겟 필터: Character.State.Death 태그로 판단 (클래스 체크 대신)
 */
UCLASS()
class GUITARSOULSGAS_API UGA_BossAttack : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_BossAttack();

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
	// 공격 패턴 몽타주 (BP 자식에서 지정)
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TObjectPtr<UAnimMontage> AttackMontage;

	// 데미지 GE
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// 데미지 수치 (BP에서 조정)
	UPROPERTY(EditDefaultsOnly, Category = "Attack", meta = (ClampMin = "0.0"))
	float Damage = 20.f;

	// 피격 시 넉백 강도 — 0이면 넉백 없음 (일반 적과 구분)
	UPROPERTY(EditDefaultsOnly, Category = "Attack", meta = (ClampMin = "0.0"))
	float KnockbackStrength = 600.f;

private:
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	void OnWeaponHit(const FHitResult& HitResult);
	void OnAttackCollisionTagChanged(const FGameplayTag Tag, int32 NewCount);

	UPROPERTY()
	TObjectPtr<class UAbilityTask_PlayMontageAndWait> MontageTask;

	UPROPERTY()
	TObjectPtr<class AGSGASWeapon> CachedWeapon;

	FDelegateHandle HitDelegateHandle;
	FDelegateHandle CollisionTagHandle;
};
