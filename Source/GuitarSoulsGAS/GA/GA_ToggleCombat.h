// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_ToggleCombat.generated.h"

UCLASS()
class GUITARSOULSGAS_API UGA_ToggleCombat : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_ToggleCombat();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageInterrupted();

	void ApplyCombatToggle();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FGameplayTag EquipWeaponTag;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	FGameplayTag UnequipWeaponTag;

	UPROPERTY()
	TObjectPtr<class UAbilityTask_PlayMontageAndWait> MontageTask;

	UPROPERTY()
	TObjectPtr<class AGSGASCharacterBase> CachedCharacter;

	bool bWasEquipping = false;
};
