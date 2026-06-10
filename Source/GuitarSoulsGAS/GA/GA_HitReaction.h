// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_HitReaction.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API UGA_HitReaction : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_HitReaction();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "HitReaction")
	TObjectPtr<class UAnimMontage> DefaultHitReactionMontage;

	UPROPERTY(EditDefaultsOnly, Category = "HitReaction")
	FGameplayTag HitReactionTag;

	// 피격 캐릭터 측 사운드 (신음 등). 파티클은 WeaponCollisionComponent에서 처리
	UPROPERTY(EditDefaultsOnly, Category = "HitReaction|FX")
	TObjectPtr<class USoundBase> HitSound;

	UPROPERTY()
	TObjectPtr<class UAbilityTask_PlayMontageAndWait> MontageTask;

	UFUNCTION()
	void OnMontageEnd();

	UFUNCTION()
	void OnMontageInterrupted();

private:
	static FName GetHitDirectionSectionName(
		const FVector& VictimLocation,
		const FRotator& VictimRotation,
		const FVector& AttackerLocation);

	// EndAbility 복원용 — 피격 전 값 저장
	bool bSavedOrientRotationToMovement = true;
};
