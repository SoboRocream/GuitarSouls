// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_Interact.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API UGA_Interact : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_Interact();

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = Interact)
	float InteractRadius = 100.f;

	// 상호작용 탐지 구체를 화면에 그린다. Shipping이 아닌 빌드에서는 켜두면 실제로 보이므로
	// 전시/시연 빌드에서는 반드시 false 유지.
	UPROPERTY(EditDefaultsOnly, Category = Interact)
	bool bDrawDebug = false;

	
};
