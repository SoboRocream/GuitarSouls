// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_EnemyAttack.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API UGA_EnemyAttack : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_EnemyAttack();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	void ResolveAttackRequest(const FGameplayEventData* TriggerEventData);
	FName GetCurrentSectionName() const;
	void PlayCurrentAttackSection();
	void ApplyStaminaCost();
	bool CanContinueCombo() const;

	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	void OnWeaponHit(const FHitResult& HitResult);
	void OnAttackCollisionTagChanged(const FGameplayTag Tag, int32 NewCount);
	void OnComboWindowTagChanged(const FGameplayTag Tag, int32 NewCount);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	FGameplayTag LightAttackTag;

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

	static const TArray<FName> LightComboSectionNames;

	int32 CurrentComboIndex = 0;
	int32 RequestedComboCount = 1;
	int32 ResolvedMaxComboCount = 1;

	FGameplayTag ResolvedAttackTag;

	FDelegateHandle HitDelegateHandle;
	FDelegateHandle CollisionTagHandle;
	FDelegateHandle ComboWindowTagHandle;
};
