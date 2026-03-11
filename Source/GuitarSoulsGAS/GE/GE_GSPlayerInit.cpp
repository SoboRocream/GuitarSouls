// Fill out your copyright notice in the Description page of Project Settings.


#include "GE/GE_GSPlayerInit.h"
#include "Attribute/GSAttributeSet.h"

UGE_GSPlayerInit::UGE_GSPlayerInit()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// ── MaxHealth ────────────────────────────────────────────────────────────
	{
		FGameplayModifierInfo Mod;
		Mod.Attribute = UGSAttributeSet::GetMaxHealthAttribute();
		Mod.ModifierOp = EGameplayModOp::Override;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(1000.f));
		Modifiers.Add(Mod);
	}

	// ── Health ───────────────────────────────────────────────────────────────
	{
		FGameplayModifierInfo Mod;
		Mod.Attribute = UGSAttributeSet::GetHealthAttribute();
		Mod.ModifierOp = EGameplayModOp::Override;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(1000.f));
		Modifiers.Add(Mod);
	}

	// ── MaxStamina ───────────────────────────────────────────────────────────
	{
		FGameplayModifierInfo Mod;
		Mod.Attribute = UGSAttributeSet::GetMaxStaminaAttribute();
		Mod.ModifierOp = EGameplayModOp::Override;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(100.f));
		Modifiers.Add(Mod);
	}

	// ── Stamina ──────────────────────────────────────────────────────────────
	{
		FGameplayModifierInfo Mod;
		Mod.Attribute = UGSAttributeSet::GetStaminaAttribute();
		Mod.ModifierOp = EGameplayModOp::Override;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(100.f));
		Modifiers.Add(Mod);
	}

	// ── Damage ───────────────────────────────────────────────────────────────
	{
		FGameplayModifierInfo Mod;
		Mod.Attribute = UGSAttributeSet::GetDamageAttribute();
		Mod.ModifierOp = EGameplayModOp::Override;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(100.f));
		Modifiers.Add(Mod);
	}

	// ── MaxPotionCount ───────────────────────────────────────────────────────
	{
		FGameplayModifierInfo Mod;
		Mod.Attribute = UGSAttributeSet::GetMaxPotionCountAttribute();
		Mod.ModifierOp = EGameplayModOp::Override;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(5.f));
		Modifiers.Add(Mod);
	}

	// ── PotionCount ──────────────────────────────────────────────────────────
	{
		FGameplayModifierInfo Mod;
		Mod.Attribute = UGSAttributeSet::GetPotionCountAttribute();
		Mod.ModifierOp = EGameplayModOp::Override;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(5.f));
		Modifiers.Add(Mod);
	}
}
