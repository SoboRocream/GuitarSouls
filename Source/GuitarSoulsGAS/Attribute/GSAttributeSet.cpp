// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/GSAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "GuitarSoulsGAS.h"
#include "Tags/GSGASGameplayTags.h"

UGSAttributeSet::UGSAttributeSet()
{
	InitHealth(1000.f);
	InitMaxHealth(1000.f);
	InitStamina(100.f);
	InitMaxStamina(100.f);
	InitDamage(0.f);
	InitAttackPower(1.f);
	InitDefense(0.f);
	InitPotionCount(5.f);
	InitMaxPotionCount(5.f);
}

void UGSAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxStamina());
	}
	else if (Attribute == GetPotionCountAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxPotionCount());
	}
}

void UGSAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));

		if (GetHealth() <= 0.f)
		{
			OnOutOfHealth.Broadcast();
		}
	}
	else if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Damage 어트리뷰트 경유 방식
		// GE에서 Damage에 값 설정 → 여기서 Health에 반영 후 Damage 초기화
		const float RawDamage = GetDamage();
		GSGAS_LOG(LogGSGAS, Warning, TEXT("PostGE Damage received: %.1f on %s"),
		RawDamage, *GetOwningActor()->GetName());
		SetDamage(0.f);
 
		if (RawDamage > 0.f)
		{
			// TODO: ExecutionCalculation 도입 시 방어 수식 적용
			// 현재는 Defense = 0 으로 처리 (VerticalSlice 스코프)
			const float NewHealth = FMath::Clamp(GetHealth() - RawDamage, 0.f, GetMaxHealth());
			SetHealth(NewHealth);
 
			if (NewHealth <= 0.f)
			{
				OnOutOfHealth.Broadcast();
			}
			else
			{
				FGameplayEventData EventData;

				AActor* Attacker = Data.EffectSpec.GetEffectContext().GetEffectCauser();
				if (!Attacker)
				{
					if (UAbilitySystemComponent* SourceASC = Data.EffectSpec.GetEffectContext().GetInstigatorAbilitySystemComponent())
					{
						Attacker = SourceASC->GetAvatarActor();
					}
				}

				EventData.Instigator = Attacker;
				EventData.ContextHandle = Data.EffectSpec.GetEffectContext();
				// 넉백 강도: GA_BossAttack이 설정한 경우에만 > 0, 나머지는 0 (fallback)
				EventData.EventMagnitude = Data.EffectSpec.GetSetByCallerMagnitude(
					GSGASGameplayTags::Data_KnockbackStrength, false, 0.f);

				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
					GetOwningActor(),
					GSGASGameplayTags::Character_Action_HitReaction,
					EventData);
			}
		}
	}
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));

		if (GetStamina() <= 0.f)
		{
			OnOutOfStamina.Broadcast();
		}
	}
	else if (Data.EvaluatedData.Attribute == GetPotionCountAttribute())
	{
		SetPotionCount(FMath::Clamp(GetPotionCount(), 0.f, GetMaxPotionCount()));

		if (GetPotionCount() <= 0.f)
		{
			OnOutOfPotion.Broadcast();
		}
	}
}


