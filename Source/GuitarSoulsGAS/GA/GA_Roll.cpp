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
	
	if (!MovementInput.IsNearlyZero())
	{
		ApplyRollRotation(MovementInput, Character);
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

	const FRotator YawRotation(0.f, Character->GetControlRotation().Yaw, 0.f);
	const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	const FVector InputDir = (ForwardDir * MovementInput.X + RightDir * MovementInput.Y).GetSafeNormal();

	if (!InputDir.IsNearlyZero())
	{
		Character->SetActorRotation(InputDir.Rotation());
	}
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
