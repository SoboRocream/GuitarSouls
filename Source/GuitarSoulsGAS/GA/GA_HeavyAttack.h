// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_HeavyAttack.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API UGA_HeavyAttack : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_HeavyAttack();
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

protected:
	void PlayAttackMontage();
	void ApplyStaminaCost();
	void ApplyDamageToTarget(const FHitResult& HitResult);

	UFUNCTION()
	void OnMontageCompleted();
 
	UFUNCTION()
	void OnMontageInterrupted();
 
	void OnWeaponHit(const FHitResult& HitResult);
	void OnAttackCollisionTagChanged(const FGameplayTag Tag, int32 NewCount);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	FGameplayTag HeavyAttackTag;
	
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
 
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<UGameplayEffect> StaminaCostEffectClass;
 
	UPROPERTY()
	TObjectPtr<class UAbilityTask_PlayMontageAndWait> MontageTask;

	UPROPERTY()
	TObjectPtr<class AGSGASWeapon> CachedWeapon;
	
	FDelegateHandle HitDelegateHandle;
	FDelegateHandle CollisionTagHandle;
};
