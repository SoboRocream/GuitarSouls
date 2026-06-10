// Fill out your copyright notice in the Description page of Project Settings.

#include "GA/GA_BossPhaseTransition.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
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
	// GA가 외부에서 강제 취소됐고 아직 페이즈 전환이 완료되지 않은 경우
	if (bWasCancelled && !bTransitionFinished)
	{
		// TransitionMontage 유무와 관계없이 ActivateAbility에서 추가한 태그는 반드시 제거
		if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
		{
			ASC->RemoveLooseGameplayTag(GSGASGameplayTags::Character_State_Invincible);
			ASC->RemoveLooseGameplayTag(GSGASGameplayTags::Boss_State_PhaseTransition);
		}

		// 몽타주 연출 보장은 가능한 경우에만
		if (TransitionMontage)
		{
			if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
			{
				if (UAnimInstance* AnimInstance = Character->GetMesh() ? Character->GetMesh()->GetAnimInstance() : nullptr)
				{
					AnimInstance->Montage_Play(TransitionMontage);

					FOnMontageEnded EndDelegate;
					EndDelegate.BindUObject(this, &UGA_BossPhaseTransition::OnMontageEndedDirect);
					AnimInstance->Montage_SetEndDelegate(EndDelegate, TransitionMontage);

					GSGAS_LOG(LogGSGAS, Log, TEXT("GA_BossPhaseTransition: replaying montage after cancel."));
				}
			}
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_BossPhaseTransition::OnMontageEnd()
{
	FinishTransition();
}

void UGA_BossPhaseTransition::OnMontageInterrupted()
{
	// Task 경로에서 몽타주가 끊힌 경우 — FinishTransition이 EndAbility → AnimInstance 직접 재생 경로로 이어짐
	GSGAS_LOG(LogGSGAS, Warning, TEXT("GA_BossPhaseTransition: Montage interrupted via Task."));
	FinishTransition();
}

void UGA_BossPhaseTransition::OnMontageEndedDirect(UAnimMontage* Montage, bool bInterrupted)
{
	// GA 강제 취소 후 직접 재생한 몽타주가 끝난 경우
	if (bInterrupted)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("GA_BossPhaseTransition: Direct montage also interrupted, forcing transition."));
	}
	FinishTransition();
}

void UGA_BossPhaseTransition::FinishTransition()
{
	if (bTransitionFinished) return;
	bTransitionFinished = true;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		ASC->AddLooseGameplayTag(GSGASGameplayTags::Boss_Phase_2);
		ASC->RemoveLooseGameplayTag(GSGASGameplayTags::Character_State_Invincible);
		ASC->RemoveLooseGameplayTag(GSGASGameplayTags::Boss_State_PhaseTransition);
		GSGAS_LOG(LogGSGAS, Log, TEXT("GA_BossPhaseTransition: Phase 2 activated."));
	}

	// GA가 아직 활성 상태일 때만 (정상 Task 경로) EndAbility 호출
	// 강제 취소 경로에서는 이미 EndAbility가 완료된 상태
	if (IsActive())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
	}
}

void UGA_BossPhaseTransition::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility, true);
}
