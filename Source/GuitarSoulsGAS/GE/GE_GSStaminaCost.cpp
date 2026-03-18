// Fill out your copyright notice in the Description page of Project Settings.


#include "GE/GE_GSStaminaCost.h"
#include "Attribute/GSAttributeSet.h"
#include "Tags/GSGASGameplayTags.h"

UGE_GSStaminaCost::UGE_GSStaminaCost()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	FGameplayModifierInfo Mod;
	Mod.Attribute = UGSAttributeSet::GetStaminaAttribute();
	Mod.ModifierOp = EGameplayModOp::Additive;

	FSetByCallerFloat  SetByCaller;
	SetByCaller.DataTag = GSGASGameplayTags::Data_StaminaCost;
	Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);

	Modifiers.Add(Mod);
}
