// Fill out your copyright notice in the Description page of Project Settings.

#include "GA/GA_UsePotion.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Attribute/GSAttributeSet.h"
#include "GE/GE_GSHeal.h"
#include "GE/GE_GSPotionCost.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GuitarSoulsGAS.h"
#include "Tags/GSGASGameplayTags.h"

UGA_UsePotion::UGA_UsePotion()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;

	// 포션 소모 GE를 코스트로 등록 — CommitAbility() 호출 시 PotionCount -1 적용
	CostGameplayEffectClass = UGE_GSPotionCost::StaticClass();

	ActivationBlockedTags.AddTag(GSGASGameplayTags::Character_State_Death);
	ActivationBlockedTags.AddTag(GSGASGameplayTags::Character_State_Toggling);

	// 발동 중 이 태그가 자동으로 부착됨
	// → Move()에서 이동 차단 / 다른 GA의 ActivationBlockedTags에서 행동 차단
	ActivationOwnedTags.AddTag(GSGASGameplayTags::Character_Action_UsePotion);
}

void UGA_UsePotion::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	// 포션 잔여 수량 확인 (0이면 발동 취소)
	const float CurrentPotionCount = ASC->GetNumericAttribute(UGSAttributeSet::GetPotionCountAttribute());
	if (CurrentPotionCount <= 0.f)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("GA_UsePotion: No potions remaining."));
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	// 이동 차단 — EndAbility에서 Walking으로 원복
	if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
	{
		if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
		{
			MoveComp->DisableMovement();
			MoveComp->StopMovementImmediately();
		}
	}

	// CommitAbility: PotionCount -1 적용 (GE_GSPotionCost)
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("GA_UsePotion: CommitAbility failed."));
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	// 힐 GE 즉시 적용 (Health += HealAmount)
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(UGE_GSHeal::StaticClass(), 1.f, ContextHandle);
	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(GSGASGameplayTags::Data_HealAmount, HealAmount);
		ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		GSGAS_LOG(LogGSGAS, Log, TEXT("GA_UsePotion: Healed %.1f HP. Remaining potions: %.0f"),
			HealAmount, CurrentPotionCount - 1.f);
	}
	else
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("GA_UsePotion: GE_GSHeal SpecHandle invalid."));
	}

	// 회복 연출 Cue 실행 (HealCueTag 설정된 경우에만)
	if (HealCueTag.IsValid())
	{
		ASC->ExecuteGameplayCue(HealCueTag, ASC->MakeEffectContext());
	}

	// 몽타주 재생 (미설정 시 즉시 종료)
	if (PotionMontage)
	{
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, PotionMontage);
		MontageTask->OnCompleted.AddDynamic(this, &UGA_UsePotion::OnMontageCompleted);
		MontageTask->OnInterrupted.AddDynamic(this, &UGA_UsePotion::OnMontageInterrupted);
		MontageTask->OnCancelled.AddDynamic(this, &UGA_UsePotion::OnMontageInterrupted);
		MontageTask->ReadyForActivation();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
	}
}

void UGA_UsePotion::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	// 이동 원복 (DisableMovement로 MOVE_None이 된 경우에만)
	if (ActorInfo)
	{
		if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
			{
				if (MoveComp->MovementMode == MOVE_None)
				{
					MoveComp->SetMovementMode(MOVE_Walking);
				}
			}
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_UsePotion::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_UsePotion::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}
