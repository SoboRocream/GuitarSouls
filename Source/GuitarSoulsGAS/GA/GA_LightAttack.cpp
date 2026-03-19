// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_LightAttack.h"
#include "Character/GSGASCharacterBase.h"
#include "Component/GSGASWeaponCollisionComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Tags/GSGASGameplayTags.h"
#include "GuitarSoulsGAS.h"

const TArray<FName> UGA_LightAttack::ComboSectionNames =
{
	TEXT("Attack_1"),
	TEXT("Attack_2"),
	TEXT("Attack_3")
};

UGA_LightAttack::UGA_LightAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_LightAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (!LightAttackTag.IsValid())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("LightAttackTag is not set. Set it in Blueprint defaults."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AGSGASCharacterBase* Character = Cast<AGSGASCharacterBase>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("AvatarActor is not AGSGASCharacterBase. EndAbility."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	WeaponData = Character->GetWeaponData();
	if (!WeaponData)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("WeaponData is null on %s. EndAbility."), *Character->GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	const FGSGASAttackData* AttackData = WeaponData->GetAttackData(LightAttackTag);
	if (!AttackData || !AttackData->Montage)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("AttackData or Montage is null. EndAbility."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UGSGASWeaponCollisionComponent* WeaponCollision = Character->GetWeaponCollision();
	if (!WeaponCollision)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("WeaponCollision is null. EndAbility."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	// Effect 적용
	ApplyStaminaCost();

	// OnHitActor 바인딩
	HitDelegateHandle = WeaponCollision->OnHitActor.AddUObject(this, &UGA_LightAttack::OnWeaponHit);

	// 태그 감지
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	CollisionTagHandle = ASC->RegisterGameplayTagEvent(GSGASGameplayTags::Character_State_AttackCollisionActive,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UGA_LightAttack::OnAttackCollisionTagChanged);

	// 몽타주 제생
	PlayCurrentComboSection();
}

void UGA_LightAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo)
	{
		if (AGSGASCharacterBase* Character = Cast<AGSGASCharacterBase>(ActorInfo->AvatarActor.Get()))
		{
			if (UGSGASWeaponCollisionComponent* WeaponCollision = Character->GetWeaponCollision())
			{
				WeaponCollision->OnHitActor.Remove(HitDelegateHandle);
 
				if (WeaponCollision->IsCollisionEnabled())
				{
					WeaponCollision->TurnOffCollision();
				}
			}
		}
	}
 
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RegisterGameplayTagEvent(
			GSGASGameplayTags::Character_State_AttackCollisionActive,
			EGameplayTagEventType::NewOrRemoved)
			.Remove(CollisionTagHandle);
	}
 
	HitDelegateHandle.Reset();
	CollisionTagHandle.Reset();
 
	CurrentComboIndex = 0;
	bHasNextComboInput = false;
	MontageTask = nullptr;
	WeaponData = nullptr;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_LightAttack::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	if (ASC->HasMatchingGameplayTag(GSGASGameplayTags::Character_State_ComboWindow))
	{
		bHasNextComboInput = true;
		GSGAS_LOG(LogGSGAS, Log, TEXT("ComboInput accepted. Index: %d"), CurrentComboIndex);
	}
}

FName UGA_LightAttack::GetCurrentSectionName() const
{
	if (ComboSectionNames.IsValidIndex(CurrentComboIndex))
	{
		return ComboSectionNames[CurrentComboIndex];
	}

	GSGAS_LOG(LogGSGAS, Warning, TEXT("Invalid ComboIndex: %d. Falling back to Attack_1."), CurrentComboIndex);
	return ComboSectionNames[0];
}

void UGA_LightAttack::PlayCurrentComboSection()
{
	const FGSGASAttackData* AttackData = WeaponData->GetAttackData(LightAttackTag);
	
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		TEXT("LightAttackMontage"),
		AttackData->Montage,
		1.f,
		GetCurrentSectionName());

	MontageTask->OnCompleted.AddDynamic(this, &UGA_LightAttack::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_LightAttack::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_LightAttack::OnMontageInterrupted);
	MontageTask->ReadyForActivation();

	GSGAS_LOG(LogGSGAS, Log, TEXT("Playing section: %s"), *GetCurrentSectionName().ToString());
}

void UGA_LightAttack::ApplyStaminaCost()
{
	if (!StaminaCostEffectClass)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("StaminaCostEffectClass is null."));
		return;
	}
 
	const FGSGASAttackData* AttackData = WeaponData->GetAttackData(LightAttackTag);
	if (!AttackData)
	{
		return;
	}
 
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(StaminaCostEffectClass);
	if (!SpecHandle.IsValid())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("StaminaCost SpecHandle is invalid."));
		return;
	}
 
	SpecHandle.Data->SetSetByCallerMagnitude(GSGASGameplayTags::Data_StaminaCost, -AttackData->StaminaCost);
 
	const FActiveGameplayEffectHandle ActiveHandle = ApplyGameplayEffectSpecToOwner(
		GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), SpecHandle);
 
	if (!ActiveHandle.WasSuccessfullyApplied())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("StaminaCost GE failed to apply."));
		return;
	}
 
	GSGAS_LOG(LogGSGAS, Log, TEXT("StaminaCost applied: %.1f"), AttackData->StaminaCost);
}

void UGA_LightAttack::ApplyDamageToTarget(const FHitResult& HitResult)
{
	if (!DamageEffectClass)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("DamageEffectClass is null."));
		return;
	}
 
	const FGSGASAttackData* AttackData = WeaponData->GetAttackData(LightAttackTag);
	if (!AttackData)
	{
		return;
	}
 
	AActor* HitActor = HitResult.GetActor();
	if (!HitActor)
	{
		return;
	}
 
	UAbilitySystemComponent* TargetASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
	if (!TargetASC)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("TargetASC is null on %s."), *HitActor->GetName());
		return;
	}
 
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
	if (!SpecHandle.IsValid())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("Damage SpecHandle is invalid."));
		return;
	}
 
	const float FinalDamage = WeaponData->BaseDamage * AttackData->DamageMultiplier;
	SpecHandle.Data->SetSetByCallerMagnitude(GSGASGameplayTags::Data_Damage, FinalDamage);
 
	FGameplayAbilityTargetDataHandle TargetDataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* TargetData =
		new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
	TargetDataHandle.Add(TargetData);
 
	const TArray<FActiveGameplayEffectHandle> ActiveHandles = ApplyGameplayEffectSpecToTarget(
		GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(),
		GetCurrentActivationInfo(), SpecHandle, TargetDataHandle);
 
	if (ActiveHandles.IsEmpty())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("Damage GE failed to apply on %s."), *HitActor->GetName());
		return;
	}
 
	GSGAS_LOG(LogGSGAS, Log, TEXT("Damage applied to %s: %.1f"), *HitActor->GetName(), FinalDamage);
}

void UGA_LightAttack::OnMontageCompleted()
{
	if (bHasNextComboInput && CurrentComboIndex + 1 < ComboSectionNames.Num())
	{
		bHasNextComboInput = false;
		CurrentComboIndex++;
		ApplyStaminaCost();
		PlayCurrentComboSection();
		GSGAS_LOG(LogGSGAS, Log, TEXT("Combo continue. Index: %d"), CurrentComboIndex);
	}
	else
	{
		GSGAS_LOG(LogGSGAS, Log, TEXT("Combo end."));
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(),
			GetCurrentActivationInfo(), true, false);
	}
}

void UGA_LightAttack::OnMontageInterrupted()
{
	GSGAS_LOG(LogGSGAS, Log, TEXT("Montage interrupted."));
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(),
		GetCurrentActivationInfo(), true, true);
}

void UGA_LightAttack::OnWeaponHit(const FHitResult& HitResult)
{
	ApplyDamageToTarget(HitResult);
}

void UGA_LightAttack::OnAttackCollisionTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	AGSGASCharacterBase* Character = Cast<AGSGASCharacterBase>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return;
	}
 
	UGSGASWeaponCollisionComponent* WeaponCollision = Character->GetWeaponCollision();
	if (!WeaponCollision)
	{
		return;
	}
 
	if (NewCount > 0)
	{
		WeaponCollision->TurnOnCollision();
	}
	else
	{
		WeaponCollision->TurnOffCollision();
	}
}
