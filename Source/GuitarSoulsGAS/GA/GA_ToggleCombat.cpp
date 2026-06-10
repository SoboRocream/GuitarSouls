// Fill out your copyright notice in the Description page of Project Settings.

#include "GA/GA_ToggleCombat.h"
#include "GuitarSoulsGAS.h"
#include "Character/GSGASCharacterBase.h"
#include "Item/GSGASWeapon.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Tags/GSGASGameplayTags.h"

UGA_ToggleCombat::UGA_ToggleCombat()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_ToggleCombat::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CachedCharacter = Cast<AGSGASCharacterBase>(ActorInfo->AvatarActor.Get());
	if (!CachedCharacter)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("ToggleCombat: AvatarActor is not AGSGASCharacterBase."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AGSGASWeapon* Weapon = CachedCharacter->GetEquippedWeapon();
	if (!Weapon)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("ToggleCombat: No weapon equipped."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	bWasEquipping = !CachedCharacter->IsCombatEnabled();
	const FGameplayTag& ActionTag = bWasEquipping ? EquipWeaponTag : UnequipWeaponTag;

	if (!ActionTag.IsValid())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("ToggleCombat: ActionTag is not set in Blueprint defaults."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	UAnimMontage* Montage = Weapon->GetMontageForTag(ActionTag);
	if (!Montage)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("ToggleCombat: No montage found for tag %s."), *ActionTag.ToString());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, TEXT("ToggleCombatMontage"), Montage, 1.f);

	MontageTask->OnCompleted.AddDynamic(this, &UGA_ToggleCombat::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_ToggleCombat::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_ToggleCombat::OnMontageInterrupted);
	MontageTask->ReadyForActivation();

	GSGAS_LOG(LogGSGAS, Log, TEXT("ToggleCombat: %s started."), bWasEquipping ? TEXT("Equip") : TEXT("Unequip"));
}

void UGA_ToggleCombat::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	MontageTask = nullptr;
	CachedCharacter = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_ToggleCombat::OnMontageCompleted()
{
	ApplyCombatToggle();
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UGA_ToggleCombat::OnMontageInterrupted()
{
	GSGAS_LOG(LogGSGAS, Log, TEXT("ToggleCombat: Montage interrupted."));
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}

void UGA_ToggleCombat::ApplyCombatToggle()
{
	if (!CachedCharacter)
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	AGSGASWeapon* Weapon = CachedCharacter->GetEquippedWeapon();
	const FGameplayTag WeaponTypeTag = Weapon ? Weapon->GetWeaponTypeTag() : FGameplayTag();

	if (bWasEquipping)
	{
		CachedCharacter->SetCombatEnabled(true);
		ASC->AddLooseGameplayTag(GSGASGameplayTags::Character_State_CombatEnabled);
		if (WeaponTypeTag.IsValid())
		{
			ASC->AddLooseGameplayTag(WeaponTypeTag);
		}
		GSGAS_LOG(LogGSGAS, Log, TEXT("ToggleCombat: Combat enabled. WeaponType=%s"), *WeaponTypeTag.ToString());
	}
	else
	{
		CachedCharacter->SetCombatEnabled(false);
		ASC->RemoveLooseGameplayTag(GSGASGameplayTags::Character_State_CombatEnabled);
		if (WeaponTypeTag.IsValid())
		{
			ASC->RemoveLooseGameplayTag(WeaponTypeTag);
		}
		GSGAS_LOG(LogGSGAS, Log, TEXT("ToggleCombat: Combat disabled. WeaponType=%s removed."), *WeaponTypeTag.ToString());
	}
}

void UGA_ToggleCombat::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility, true);
}
