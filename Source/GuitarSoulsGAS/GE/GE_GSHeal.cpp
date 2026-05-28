// Fill out your copyright notice in the Description page of Project Settings.

#include "GE/GE_GSHeal.h"
#include "Attribute/GSAttributeSet.h"
#include "Tags/GSGASGameplayTags.h"

UGE_GSHeal::UGE_GSHeal()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// Health += SetByCaller(Data.HealAmount)
	// GA에서 SetByCallerMagnitude(Data_HealAmount, 실제 회복량)으로 값 주입
	FGameplayModifierInfo Mod;
	Mod.Attribute = UGSAttributeSet::GetHealthAttribute();
	Mod.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat SetByCaller;
	SetByCaller.DataTag = GSGASGameplayTags::Data_HealAmount;
	Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);

	Modifiers.Add(Mod);
}
