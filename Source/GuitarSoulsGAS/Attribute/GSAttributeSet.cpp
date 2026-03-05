// Fill out your copyright notice in the Description page of Project Settings.


#include "Attribute/GSAttributeSet.h"
#include "GameplayEffectExtension.h"

UGSAttributeSet::UGSAttributeSet()
{
	InitHealth(1000.f);
	InitMaxHealth(1000.f);
	InitStamina(100.f);
	InitMaxStamina(100.f);
	InitDamage(100.f);
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
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		SetStamina(FMath::Clamp(GetStamina(), 0.f, GetMaxStamina()));

		if (GetStamina() <= 0.f)
		{
			OnOutOfStamina.Broadcast();
		}
	}
}


