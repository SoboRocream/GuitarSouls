// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_EnemyAttack.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Attribute/GSAttributeSet.h"
#include "Character/GSGASCharacterBase.h"
#include "Character/GSGASCharacterEnemy.h"
#include "Component/GSGASWeaponCollisionComponent.h"
#include "Item/GSGASWeapon.h"
#include "Tags/GSGASGameplayTags.h"

const TArray<FName> UGA_EnemyAttack::LightComboSectionNames =
{
	TEXT("Attack_1"),
	TEXT("Attack_2"),
	TEXT("Attack_3")
};

UGA_EnemyAttack::UGA_EnemyAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGA_EnemyAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	const AGSGASCharacterBase* Character = Cast<AGSGASCharacterBase>(ActorInfo->AvatarActor.Get());
	if (!ASC || !Character)
	{
		return false;
	}

	const UGSAttributeSet* AttributeSet = ASC->GetSet<UGSAttributeSet>();
	const AGSGASWeapon* Weapon = Character->GetEquippedWeapon();
	if (!AttributeSet || !Weapon)
	{
		// 기본적으로 Enemy는 무기가 없다면 공격 GA가 불능이어야 함
		return false;
	}

	FGameplayTag AttackTag = LightAttackTag;
	if (SourceTags)
	{
		if (SourceTags->HasTagExact(HeavyAttackTag))
		{
			AttackTag = HeavyAttackTag;
		}
		else if (SourceTags->HasTagExact(LightAttackTag))
		{
			AttackTag = LightAttackTag;
		}
	}

	const float StaminaCost = Weapon->GetStaminaCost(AttackTag);
	return AttributeSet->GetStamina() >= StaminaCost;
 }

void UGA_EnemyAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AGSGASCharacterBase* Character = Cast<AGSGASCharacterBase>(ActorInfo->AvatarActor.Get());
	if (!Character)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CachedWeapon = Character->GetEquippedWeapon();
	if (!CachedWeapon || !CachedWeapon->GetWeaponCollision())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	ResolveAttackRequest(TriggerEventData);
	CurrentComboIndex = 0;
	ApplyStaminaCost();

	if (UGSGASWeaponCollisionComponent* WeaponCollisionComponent = CachedWeapon->GetWeaponCollision())
	{
		HitDelegateHandle = WeaponCollisionComponent->OnHitActor.AddUObject(this, &UGA_EnemyAttack::OnWeaponHit);
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	CollisionTagHandle = ASC->RegisterGameplayTagEvent(
		GSGASGameplayTags::Character_State_AttackCollisionActive,
		EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UGA_EnemyAttack::OnAttackCollisionTagChanged);

	ComboWindowTagHandle = ASC->RegisterGameplayTagEvent(
		GSGASGameplayTags::Character_State_ComboWindow,
		EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &UGA_EnemyAttack::OnComboWindowTagChanged);

	PlayCurrentAttackSection();
}

void UGA_EnemyAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (CachedWeapon)
	{
		if (UGSGASWeaponCollisionComponent* WeaponCollisionComponent = CachedWeapon->GetWeaponCollision())
		{
			WeaponCollisionComponent->OnHitActor.Remove(HitDelegateHandle);
			if (WeaponCollisionComponent->IsCollisionEnabled())
			{
				WeaponCollisionComponent->TurnOffCollision();
			}
		}
	}

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->RegisterGameplayTagEvent(
			GSGASGameplayTags::Character_State_AttackCollisionActive,
			EGameplayTagEventType::NewOrRemoved)
			.Remove(CollisionTagHandle);

		ASC->RegisterGameplayTagEvent(
			GSGASGameplayTags::Character_State_ComboWindow,
			EGameplayTagEventType::NewOrRemoved)
			.Remove(ComboWindowTagHandle);
	}
	
	HitDelegateHandle.Reset();
	CollisionTagHandle.Reset();
	ComboWindowTagHandle.Reset();
	CurrentComboIndex = 0;
	RequestedComboCount = 1;
	ResolvedMaxComboCount = 1;
	ResolvedAttackTag = FGameplayTag();
	MontageTask = nullptr;
	CachedWeapon = nullptr;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_EnemyAttack::ResolveAttackRequest(const FGameplayEventData* TriggerEventData)
{
	RequestedComboCount = 1;
	ResolvedAttackTag = LightAttackTag;

	if (TriggerEventData)
	{
		if (TriggerEventData->EventTag.IsValid())
		{
			ResolvedAttackTag = TriggerEventData->EventTag;
		}

		if (TriggerEventData->EventMagnitude > 0.f)
		{
			RequestedComboCount = FMath::RoundToInt(TriggerEventData->EventMagnitude);
		}
	}

	if (ResolvedAttackTag.MatchesTagExact(HeavyAttackTag))
	{
		ResolvedMaxComboCount = 1;
	}
	else
	{
		ResolvedAttackTag = LightAttackTag;
		ResolvedMaxComboCount = FMath::Clamp(RequestedComboCount, 1, LightComboSectionNames.Num());
	}
}

FName UGA_EnemyAttack::GetCurrentSectionName() const
{
	if (ResolvedAttackTag.MatchesTagExact(HeavyAttackTag))
	{
		return TEXT("Attack_Heavy");
	}

	if (LightComboSectionNames.IsValidIndex(CurrentComboIndex))
	{
		return LightComboSectionNames[CurrentComboIndex];
	}

	return LightComboSectionNames[0];
}

void UGA_EnemyAttack::PlayCurrentAttackSection()
{
	if (!CachedWeapon)
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
		return;
	}

	UAnimMontage* Montage = CachedWeapon->GetMontageForTag(ResolvedAttackTag);
	if (!Montage)
	{
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
		return;
	}

	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		TEXT("EnemyAttackMontage"),
		Montage,
		1.f,
		GetCurrentSectionName());

	MontageTask->OnCompleted.AddDynamic(this, &UGA_EnemyAttack::OnMontageCompleted);
	MontageTask->OnInterrupted.AddDynamic(this, &UGA_EnemyAttack::OnMontageInterrupted);
	MontageTask->OnCancelled.AddDynamic(this, &UGA_EnemyAttack::OnMontageInterrupted);
	MontageTask->ReadyForActivation();
}

void UGA_EnemyAttack::ApplyStaminaCost()
{
	if (!StaminaCostEffectClass || !CachedWeapon)
	{
		return;
	}

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(StaminaCostEffectClass);
	if (!SpecHandle.IsValid())
	{
		return;
	}

	const float StaminaCost = CachedWeapon->GetStaminaCost(ResolvedAttackTag);
	SpecHandle.Data->SetSetByCallerMagnitude(GSGASGameplayTags::Data_StaminaCost, -StaminaCost);

	ApplyGameplayEffectSpecToOwner(
		GetCurrentAbilitySpecHandle(),
		GetCurrentActorInfo(),
		GetCurrentActivationInfo(),
		SpecHandle);
}

bool UGA_EnemyAttack::CanContinueCombo() const
{
	if (!CachedWeapon)
	{
		return false;
	}

	const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return false;
	}

	const UGSAttributeSet* AttributeSet = ASC->GetSet<UGSAttributeSet>();
	if (!AttributeSet)
	{
		return false;
	}

	const float StaminaCost = CachedWeapon->GetStaminaCost(ResolvedAttackTag);
	return AttributeSet->GetStamina() >= StaminaCost;
}

void UGA_EnemyAttack::OnMontageCompleted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, false);
}

void UGA_EnemyAttack::OnMontageInterrupted()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
}

void UGA_EnemyAttack::OnWeaponHit(const FHitResult& HitResult)
{
	if (!DamageEffectClass || !CachedWeapon)
	{
		return;
	}

	AActor* HitActor = HitResult.GetActor();
	if (!HitActor)
	{
		return;
	}
	
	if (HitActor->IsA<AGSGASCharacterEnemy>())
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
	if (!TargetASC)
	{
		return;
	}

	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
	if (!SpecHandle.IsValid())
	{
		return;
	}

	const float FinalDamage = CachedWeapon->GetFinalDamage(ResolvedAttackTag);
	SpecHandle.Data->SetSetByCallerMagnitude(GSGASGameplayTags::Data_Damage, FinalDamage);

	FGameplayAbilityTargetDataHandle TargetDataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
	TargetDataHandle.Add(TargetData);

	ApplyGameplayEffectSpecToTarget(
		GetCurrentAbilitySpecHandle(),
		GetCurrentActorInfo(),
		GetCurrentActivationInfo(),
		SpecHandle,
		TargetDataHandle);
}

void UGA_EnemyAttack::OnAttackCollisionTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (!CachedWeapon)
	{
		return;
	}

	UGSGASWeaponCollisionComponent* WeaponCollision = CachedWeapon->GetWeaponCollision();
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

void UGA_EnemyAttack::OnComboWindowTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (!ResolvedAttackTag.MatchesTagExact(LightAttackTag))
	{
		return;
	}

	if (NewCount != 0)
	{
		return;
	}

	if (CurrentComboIndex + 1 >= ResolvedMaxComboCount)
	{
		return;
	}

	if (!CanContinueCombo())
	{
		return;
	}

	CurrentComboIndex++;
	ApplyStaminaCost();
	MontageJumpToSection(GetCurrentSectionName());
}
