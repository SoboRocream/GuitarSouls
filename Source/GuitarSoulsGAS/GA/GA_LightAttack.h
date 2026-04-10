// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_LightAttack.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API UGA_LightAttack : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGA_LightAttack();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	FName GetCurrentSectionName() const;
	void PlayCurrentComboSection();
	void ApplyStaminaCost();

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
	TSubclassOf<UGameplayEffect> DamageEffectClass;
 
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	TSubclassOf<UGameplayEffect> StaminaCostEffectClass;
 
	UPROPERTY()
	TObjectPtr<class UAbilityTask_PlayMontageAndWait> MontageTask;

	static const TArray<FName> ComboSectionNames;
 
	int32 CurrentComboIndex = 0;

	bool bIsComboInputQueued = false;
	FDelegateHandle ComboWindowTagHandle;

	UPROPERTY()
	TObjectPtr<class AGSGASWeapon> CachedWeapon;
	
	// EndAbility 시 반드시 해제
	FDelegateHandle HitDelegateHandle;
	FDelegateHandle CollisionTagHandle;
};
