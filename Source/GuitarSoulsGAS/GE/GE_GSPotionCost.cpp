// Fill out your copyright notice in the Description page of Project Settings.

#include "GE/GE_GSPotionCost.h"
#include "Attribute/GSAttributeSet.h"

UGE_GSPotionCost::UGE_GSPotionCost()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// PotionCount -= 1 (고정값)
	// PreAttributeChange에서 0 미만 클램핑, PostGE에서 OnOutOfPotion 브로드캐스트
	FGameplayModifierInfo Mod;
	Mod.Attribute = UGSAttributeSet::GetPotionCountAttribute();
	Mod.ModifierOp = EGameplayModOp::Additive;
	Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(-1.f));

	Modifiers.Add(Mod);
}
