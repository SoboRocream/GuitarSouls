// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/GA_HeavyAttack.h"
#include "GuitarSoulsGAS.h"
#include "Character/GSGASCharacterBase.h"
#include "Character/GSGASCharacterPlayer.h"
#include "Component/GSGASBerserkComponent.h"
#include "Item/GSGASWeapon.h"
#include "Component/GSGASWeaponCollisionComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Attribute/GSAttributeSet.h"
#include "Tags/GSGASGameplayTags.h"
#include "Engine/Engine.h"

UGA_HeavyAttack::UGA_HeavyAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UGA_HeavyAttack::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
        {
            return false;
        }
    
        const UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
        if (!ASC) return false;
    
        const UGSAttributeSet* AttributeSet = ASC->GetSet<UGSAttributeSet>();
        if (!AttributeSet) return false;
    
        const AGSGASCharacterBase* Character = Cast<AGSGASCharacterBase>(ActorInfo->AvatarActor.Get());
        if (!Character) return false;
    
        const AGSGASWeapon* Weapon = Character->GetEquippedWeapon();
        if (!Weapon) return false;
    
        const float StaminaCost = Weapon->GetStaminaCost(HeavyAttackTag);
        const bool bEnough = AttributeSet->GetStamina() >= StaminaCost;
    
        if (!bEnough)
        {
            GSGAS_LOG(LogGSGAS, Log, TEXT("HeavyAttack blocked: stamina %.1f < cost %.1f"),
                AttributeSet->GetStamina(), StaminaCost);
        }
    
        return bEnough;
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
 
	CachedWeapon = Character->GetEquippedWeapon();
	if (!CachedWeapon)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("EquippedWeapon is null on %s. EndAbility."), *Character->GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
 
	UGSGASWeaponCollisionComponent* WeaponCollision = CachedWeapon->GetWeaponCollision();
	if (!WeaponCollision)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("WeaponCollision is null. EndAbility."));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
 
	ApplyStaminaCost();
 
	HitDelegateHandle = WeaponCollision->OnHitActor.AddUObject(this, &UGA_HeavyAttack::OnWeaponHit);
 
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	CollisionTagHandle = ASC->RegisterGameplayTagEvent(
		GSGASGameplayTags::Character_State_AttackCollisionActive,
		EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &UGA_HeavyAttack::OnAttackCollisionTagChanged);
 
	PlayAttackMontage();
}

void UGA_HeavyAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
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

		ASC->SetLooseGameplayTagCount(GSGASGameplayTags::Character_State_AttackCollisionActive, 0);
	}

	HitDelegateHandle.Reset();
	CollisionTagHandle.Reset();
	MontageTask = nullptr;
	CachedWeapon = nullptr;
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_HeavyAttack::PlayAttackMontage()
{
	UAnimMontage* Montage = CachedWeapon->GetMontageForTag(HeavyAttackTag);
	if (!Montage)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("Montage is null for tag: %s. EndAbility."), *HeavyAttackTag.ToString());
		EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true, true);
		return;
	}
 
	MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		TEXT("HeavyAttackMontage"),
		Montage,
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
 
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(StaminaCostEffectClass);
	if (!SpecHandle.IsValid())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("StaminaCost SpecHandle is invalid."));
		return;
	}
 
	const float StaminaCost = CachedWeapon->GetStaminaCost(HeavyAttackTag);
	SpecHandle.Data->SetSetByCallerMagnitude(GSGASGameplayTags::Data_StaminaCost, -StaminaCost);
 
	const FActiveGameplayEffectHandle ActiveHandle = ApplyGameplayEffectSpecToOwner(
		GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), SpecHandle);
 
	if (!ActiveHandle.WasSuccessfullyApplied())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("StaminaCost GE failed to apply."));
		return;
	}
 
	GSGAS_LOG(LogGSGAS, Log, TEXT("StaminaCost applied: %.1f"), StaminaCost);
}

void UGA_HeavyAttack::ApplyDamageToTarget(const FHitResult& HitResult)
{
	if (!DamageEffectClass)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("DamageEffectClass is null."));
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
 
	// [전시용] 광폭화 방식 분기: 컴포넌트 모드면 배율을 데미지에 직접 곱해 주입한다.
	// (GAS 모드는 AttackPower 어트리뷰트를 ExecCalc가 자동 반영하므로 원본 데미지 그대로)
	AGSGASCharacterPlayer* OwnerPlayer = Cast<AGSGASCharacterPlayer>(GetAvatarActorFromActorInfo());
	const bool bComponentBerserk = OwnerPlayer && OwnerPlayer->IsUsingComponentBerserk();

	float FinalDamage = CachedWeapon->GetFinalDamage(HeavyAttackTag);
	float AppliedBerserkMult = 1.f;
	if (bComponentBerserk)
	{
		if (UGSGASBerserkComponent* Berserk = OwnerPlayer->GetBerserkComponent())
		{
			AppliedBerserkMult = Berserk->GetDamageMultiplier();
			FinalDamage *= AppliedBerserkMult;
		}
	}
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

	// 적중 성공 시 광폭화 스택 처리 — 모드별 분기
	if (bComponentBerserk)
	{
		// 컴포넌트 방식: 스택 누적/타이머를 컴포넌트가 수동 관리
		if (UGSGASBerserkComponent* Berserk = OwnerPlayer->GetBerserkComponent())
		{
			Berserk->OnHitLanded();
			if (GEngine)
			{
				// [임시 검증] 이번 타격에 적용된 배율·데미지 + 다음 타격 스택
				GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Orange,
					FString::Printf(TEXT("Berserk[COMPONENT] x%.1f  Dmg:%.0f  (next Stacks:%d)"),
						AppliedBerserkMult, FinalDamage, Berserk->GetCurrentStacks()));
			}
		}
	}
	else if (OnHitSelfEffectClass)
	{
		// GAS 방식: 온-히트 버프 GE를 자기 자신에게 적용 (스택/지속/만료는 GE 데이터가 처리)
		FGameplayEffectSpecHandle SelfSpec = MakeOutgoingGameplayEffectSpec(OnHitSelfEffectClass);
		if (SelfSpec.IsValid())
		{
			ApplyGameplayEffectSpecToOwner(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(),
				GetCurrentActivationInfo(), SelfSpec);

			// 디버그: 현재 스택(타수)을 화면에 표시
			if (UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo())
			{
				const int32 Stacks = OwnerASC->GetGameplayEffectCount(OnHitSelfEffectClass, nullptr);
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Orange,
						FString::Printf(TEXT("Berserk[GAS] Stacks: %d"), Stacks));
				}
			}
		}
	}
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
		CachedWeapon->ActivateTrail();
	}
	else
	{
		WeaponCollision->TurnOffCollision();
		CachedWeapon->DeactivateTrail();
	}
}

void UGA_HeavyAttack::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility, true);
}
