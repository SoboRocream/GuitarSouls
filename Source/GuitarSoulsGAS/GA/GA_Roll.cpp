// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_Roll.h"

#include "AbilitySystemComponent.h"
#include "GuitarSoulsGAS.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/GSGASCharacterPlayer.h"
#include "Tags/GSGASGameplayTags.h"

UGA_Roll::UGA_Roll()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
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
	const bool bIsLockOn = Character->GetAbilitySystemComponent()->HasMatchingGameplayTag(GSGASGameplayTags::Character_State_LockOn);

	UAnimMontage* SelectedMontage = SelectMontage(MovementInput, bIsLockOn);
	if (!SelectedMontage)
	{
		// 백스텝 에셋 미보유 등 몽타주 없는 경우 조기 종료
		GSGAS_LOG(LogGSGAS, Warning, TEXT("Roll montage is null. EndAbility."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!MovementInput.IsNearlyZero())
	{
		ApplyRollRotation(MovementInput, Character);
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("RollMontage"), SelectedMontage, 1.f);

	MontageTask->OnCompleted.AddDynamic(this, &UGA_Roll::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_Roll::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_Roll::OnMontageInterrupted);
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

UAnimMontage* UGA_Roll::SelectMontage(const FVector2D& MovementInput, bool bIsLockOn) const
{
	if (MovementInput.IsNearlyZero())
	{
		if (bIsLockOn)
		{
			// TODO: 백스텝 에셋 추가 후 BackwardMontage 반환
			// return BackwardMontage;
			GSGAS_LOG(LogGSGAS, Log, TEXT("Backstep: no asset yet."));
			return nullptr;
		}
		return ForwardMontage;
	}

	const float AbsX = FMath::Abs(MovementInput.X);
	const float AbsY = FMath::Abs(MovementInput.Y);
	
	if (AbsX >= AbsY)
	{
		return MovementInput.X >= 0.f ? ForwardMontage : BackwardMontage;
	}
	else
	{
		return MovementInput.Y >= 0.f ? RightMontage : LeftMontage;
	}
}

void UGA_Roll::ApplyRollRotation(const FVector2D& MovementInput, ACharacter* Character) const
{
	if (!Character || !Character->GetController()) return;

	const FRotator YawRotation(0.f, Character->GetControlRotation().Yaw, 0.f);
	const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	const FVector InputDir =
		(ForwardDir * MovementInput.X + RightDir * MovementInput.Y).GetSafeNormal();

	if (!InputDir.IsNearlyZero())
	{
		Character->SetActorRotation(InputDir.Rotation());
	}
}
