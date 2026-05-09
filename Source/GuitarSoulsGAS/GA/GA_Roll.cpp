// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_Roll.h"

#include "AbilitySystemComponent.h"
#include "GuitarSoulsGAS.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Attribute/GSAttributeSet.h"
#include "Character/GSGASCharacterPlayer.h"
#include "Tags/GSGASGameplayTags.h"

UGA_Roll::UGA_Roll()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGA_Roll::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC) return false;

	const UGSAttributeSet* AttributeSet = ASC->GetSet<UGSAttributeSet>();
	if (!AttributeSet) return false;

	const bool bEnough = AttributeSet->GetStamina() >= StaminaCost;

	if (!bEnough)
	{
		GSGAS_LOG(LogGSGAS, Log, TEXT("Roll blocked: stamina %.1f < cost %.1f"),
			AttributeSet->GetStamina(), StaminaCost);
	}

	return bEnough;
}

void UGA_Roll::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                               const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AGSGASCharacterPlayer* Character = Cast<AGSGASCharacterPlayer>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("AvatarActor is not AGSGASCharacterPlayer."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	const FVector2D MovementInput = Character->GetLastMovementInput();

	if (!MovementInput.IsNearlyZero())
	{
		ApplyRollRotation(MovementInput, Character);
	}

	if (!ForwardMontage)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("GA_Roll: ForwardMontage is not set."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("RollMontage"), ForwardMontage, 1.f);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_Roll::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_Roll::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_Roll::OnMontageInterrupted);
	
	ApplyStaminaCost();
	MontageTask->ReadyForActivation();

	GSGAS_LOG(LogGSGAS, Log, TEXT("Roll started."));
}

void UGA_Roll::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	MontageTask = nullptr;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_Roll::OnMontageCompleted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UGA_Roll::OnMontageInterrupted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}

void UGA_Roll::ApplyRollRotation(const FVector2D& MovementInput, ACharacter* Character) const
{
	if (!Character || !Character->GetController()) return;

	const float CameraYaw = Character->GetControlRotation().Yaw;

	// 카메라 기준 입력 각도: X=앞(0°), Y=오른쪽(90°)
	const float RawAngle = FMath::RadiansToDegrees(FMath::Atan2(MovementInput.Y, MovementInput.X));

	// 45° 단위 스냅 → 카메라 Yaw 더해 월드 회전으로 변환
	const float SnappedAngle = FMath::RoundToFloat(RawAngle / 45.f) * 45.f;
	Character->SetActorRotation(FRotator(0.f, CameraYaw + SnappedAngle, 0.f));
}

void UGA_Roll::ApplyStaminaCost()
{
	if (!StaminaCostEffectClass)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("StaminaCostEffectClass is null."));
		return;
	}
 
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(StaminaCostEffectClass);
	if (!SpecHandle.IsValid())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("StaminaCostEffectClass SpecHandle is invalid."));
		return;
	}
	
	SpecHandle.Data->SetSetByCallerMagnitude(GSGASGameplayTags::Data_StaminaCost, -StaminaCost);
 
	const FActiveGameplayEffectHandle ActiveHandle = ApplyGameplayEffectSpecToOwner(
		GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), SpecHandle);
 
	if (!ActiveHandle.WasSuccessfullyApplied())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("StaminaCostEffectClass GE failed to apply."));
		return;
	}
 
	GSGAS_LOG(LogGSGAS, Log, TEXT("StaminaCostEffectClass applied: %.1f"), StaminaCost);
}
