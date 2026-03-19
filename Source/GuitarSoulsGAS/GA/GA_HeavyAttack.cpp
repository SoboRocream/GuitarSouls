// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_HeavyAttack.h"
#include "Character/GSGASCharacterBase.h"
#include "Component/GSGASWeaponCollisionComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Tags/GSGASGameplayTags.h"
#include "GuitarSoulsGAS.h"

UGA_HeavyAttack::UGA_HeavyAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_HeavyAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!HeavyAttackTag.IsValid())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("HeavyAttackTag is not set. Set it in Blueprint defaults."));
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
 
	const FGSGASAttackData* AttackData = WeaponData->GetAttackData(HeavyAttackTag);
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
 
	// 스태미나 차감
	ApplyStaminaCost();
 
	// OnHitActor 바인딩
	HitDelegateHandle = WeaponCollision->OnHitActor.AddUObject(this, &UGA_HeavyAttack::OnWeaponHit);
 
	// AttackCollisionActive 태그 감지 등록
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	CollisionTagHandle = ASC->RegisterGameplayTagEvent(
		GSGASGameplayTags::Character_State_AttackCollisionActive,
		EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &UGA_HeavyAttack::OnAttackCollisionTagChanged);
 
	// 몽타주 재생
	PlayAttackMontage();
}

void UGA_HeavyAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
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
	MontageTask = nullptr;
	WeaponData = nullptr;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_HeavyAttack::PlayAttackMontage()
{
	const FGSGASAttackData* AttackData = WeaponData->GetAttackData(HeavyAttackTag);
 
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		TEXT("HeavyAttackMontage"),
		AttackData->Montage,
		1.f,
		TEXT("Attack_Heavy"));
 
	MontageTask->OnCompleted.AddDynamic(this, &UGA_HeavyAttack::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_HeavyAttack::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_HeavyAttack::OnMontageInterrupted);
	MontageTask->ReadyForActivation();
 
	GSGAS_LOG(LogGSGAS, Log, TEXT("HeavyAttack montage started."));
}

void UGA_HeavyAttack::ApplyStaminaCost()
{
	if (!StaminaCostEffectClass)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("StaminaCostEffectClass is null."));
		return;
	}

	const FGSGASAttackData* AttackData = WeaponData->GetAttackData(HeavyAttackTag);
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

void UGA_HeavyAttack::ApplyDamageToTarget(const FHitResult& HitResult)
{
	if (!DamageEffectClass)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("DamageEffectClass is null."));
		return;
	}

	const FGSGASAttackData* AttackData = WeaponData->GetAttackData(HeavyAttackTag);
	if (!AttackData)
	{
		return;
	}

	AActor* HitActor = HitResult.GetActor();
	if (!HitActor)
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
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

// ── 콜백 ───────────────────────────────────────────────────────────────────────
void UGA_HeavyAttack::OnMontageCompleted()
{
	GSGAS_LOG(LogGSGAS, Log, TEXT("HeavyAttack montage completed."));
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(),
		GetCurrentActivationInfo(), true, false);
}

void UGA_HeavyAttack::OnMontageInterrupted()
{
	GSGAS_LOG(LogGSGAS, Log, TEXT("HeavyAttack montage interrupted."));
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(),
		GetCurrentActivationInfo(), true, true);
}

void UGA_HeavyAttack::OnWeaponHit(const FHitResult& HitResult)
{
	ApplyDamageToTarget(HitResult);
}

void UGA_HeavyAttack::OnAttackCollisionTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	AGSGASCharacterBase* Character = Cast<AGSGASCharacterBase>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return;
	}

	UGSGASWeaponCollisionComponent* WeaponCollisionComponent = Character->GetWeaponCollision();
	if (!WeaponCollisionComponent)
	{
		return;
	}

	if (NewCount > 0)
	{
		WeaponCollisionComponent->TurnOnCollision();
	}
	else
	{
		WeaponCollisionComponent->TurnOffCollision();
	}
}
