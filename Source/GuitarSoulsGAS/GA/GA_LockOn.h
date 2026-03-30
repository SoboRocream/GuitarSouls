// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_LockOn.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API UGA_LockOn : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_LockOn();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
};
