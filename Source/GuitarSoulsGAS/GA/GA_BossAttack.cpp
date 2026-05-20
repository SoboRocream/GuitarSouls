// Fill out your copyright notice in the Description page of Project Settings.

#include "GA/GA_BossAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Character/GSGASCharacterBase.h"
#include "Component/GSGASWeaponCollisionComponent.h"
#include "GuitarSoulsGAS.h"
#include "Item/GSGASWeapon.h"
#include "Tags/GSGASGameplayTags.h"

UGA_BossAttack::UGA_BossAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
}

void UGA_BossAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!AttackMontage)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("GA_BossAttack: AttackMontage not set."));
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	AGSGASCharacterBase* Character = Cast<AGSGASCharacterBase>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	CachedWeapon = Character->GetEquippedWeapon();
	if (!CachedWeapon || !CachedWeapon->GetWeaponCollision())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("GA_BossAttack: No weapon or collision component on %s."), *Character->GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	// 충돌 히트 구독
	HitDelegateHandle = CachedWeapon->GetWeaponCollision()->OnHitActor.AddUObject(this, &UGA_BossAttack::OnWeaponHit);

	// 충돌 활성화 태그 구독 (AnimNotify → 태그 → 충돌 on/off)
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	CollisionTagHandle = ASC->RegisterGameplayTagEvent(
		GSGASGameplayTags::Character_State_AttackCollisionActive,
		EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &UGA_BossAttack::OnAttackCollisionTagChanged);

	// 몽타주 재생
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, NAME_None, AttackMontage);
	MontageTask->OnCompleted.AddDynamic(this, &UGA_BossAttack::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_BossAttack::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_BossAttack::OnMontageInterrupted);
	MontageTask->ReadyForActivation();
}

void UGA_BossAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (CachedWeapon)
	{
		if (UGSGASWeaponCollisionComponent* WeaponCollision = CachedWeapon->GetWeaponCollision())
		{
			WeaponCollision->OnHitActor.Remove(HitDelegateHandle);
			if (WeaponCollision->IsCollisionEnabled())
			{
				WeaponCollision->TurnOffCollision();
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
	CachedWeapon = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_BossAttack::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UGA_BossAttack::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, true);
}

void UGA_BossAttack::OnWeaponHit(const FHitResult& HitResult)
{
	if (!DamageEffectClass) return;

	AActor* HitActor = HitResult.GetActor();
	if (!HitActor) return;

	// 사망한 대상은 무시
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
	if (!TargetASC) return;

	if (TargetASC->HasMatchingGameplayTag(GSGASGameplayTags::Character_State_Death)) return;

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
	if (!SpecHandle.IsValid()) return;

	SpecHandle.Data->SetSetByCallerMagnitude(GSGASGameplayTags::Data_Damage, Damage);

	FGameplayAbilityTargetDataHandle TargetDataHandle;
	TargetDataHandle.Add(new FGameplayAbilityTargetData_SingleTargetHit(HitResult));

	ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo,
		SpecHandle, TargetDataHandle);
}

void UGA_BossAttack::OnAttackCollisionTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (!CachedWeapon) return;

	UGSGASWeaponCollisionComponent* WeaponCollision = CachedWeapon->GetWeaponCollision();
	if (!WeaponCollision) return;

	if (NewCount > 0)
	{
		WeaponCollision->TurnOnCollision();
	}
	else
	{
		WeaponCollision->TurnOffCollision();
	}
}
