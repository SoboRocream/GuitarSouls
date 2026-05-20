// Fill out your copyright notice in the Description page of Project Settings.

#include "GA/GA_BossPhaseTransition.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "GuitarSoulsGAS.h"
#include "Tags/GSGASGameplayTags.h"

UGA_BossPhaseTransition::UGA_BossPhaseTransition()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;

	// Boss.Phase.2 태그가 붙은 이후에는 재발동 불가
	ActivationBlockedTags.AddTag(GSGASGameplayTags::Boss_Phase_2);
	ActivationBlockedTags.AddTag(GSGASGameplayTags::Boss_State_PhaseTransition);
}

void UGA_BossPhaseTransition::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	// 무적 + 전환 중 태그 부여
	ASC->AddLooseGameplayTag(GSGASGameplayTags::Character_State_Invincible);
	ASC->AddLooseGameplayTag(GSGASGameplayTags::Boss_State_PhaseTransition);

	// GameplayCue 실행 (설정된 경우)
	if (TransitionCueTag.IsValid())
	{
		ASC->ExecuteGameplayCue(TransitionCueTag, ASC->MakeEffectContext());
	}

	// 몽타주 재생 또는 타이머 대기
	if (TransitionMontage)
	{
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, TransitionMontage);
		MontageTask->OnCompleted.AddDynamic(this, &UGA_BossPhaseTransition::OnMontageEnd);
		MontageTask->OnInterrupted.AddDynamic(this, &UGA_BossPhaseTransition::OnMontageInterrupted);
		MontageTask->OnCancelled.AddDynamic(this, &UGA_BossPhaseTransition::OnMontageInterrupted);
		MontageTask->ReadyForActivation();
	}
	else
	{
		GSGAS_LOG(LogGSGAS, Log, TEXT("GA_BossPhaseTransition: No montage set, using FallbackDelay (%.1fs)."), FallbackDelay);
		UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, FallbackDelay);
		DelayTask->OnFinish.AddDynamic(this, &UGA_BossPhaseTransition::OnMontageEnd);
		DelayTask->ReadyForActivation();
	}
}

void UGA_BossPhaseTransition::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_BossPhaseTransition::OnMontageEnd()
{
	FinishTransition();
}

void UGA_BossPhaseTransition::OnMontageInterrupted()
{
	// 인터럽트되어도 페이즈 전환은 완료 처리
	GSGAS_LOG(LogGSGAS, Warning, TEXT("GA_BossPhaseTransition: Montage interrupted, forcing phase transition."));
	FinishTransition();
}

void UGA_BossPhaseTransition::FinishTransition()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		// Phase 2 태그 부여
		ASC->AddLooseGameplayTag(GSGASGameplayTags::Boss_Phase_2);

		// 무적 + 전환 중 태그 제거
		ASC->RemoveLooseGameplayTag(GSGASGameplayTags::Character_State_Invincible);
		ASC->RemoveLooseGameplayTag(GSGASGameplayTags::Boss_State_PhaseTransition);

		GSGAS_LOG(LogGSGAS, Log, TEXT("GA_BossPhaseTransition: Phase 2 activated."));
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
