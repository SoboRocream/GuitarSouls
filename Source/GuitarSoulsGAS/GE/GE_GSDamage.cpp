// Fill out your copyright notice in the Description page of Project Settings.


#include "GE/GE_GSDamage.h"
#include "Attribute/GSAttributeSet.h"
#include "Tags/GSGASGameplayTags.h"

UGE_GSDamage::UGE_GSDamage()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// Damage 어트리뷰트에 Additive로 값을 설정
	// GA에서 SetByCallerMagnitude(GSGASGameplayTags::Data_Damage, FinalDamage)로 실제 값 주입
	FGameplayModifierInfo Mod;
	Mod.Attribute = UGSAttributeSet::GetDamageAttribute();
	Mod.ModifierOp = EGameplayModOp::Additive;
 
	FSetByCallerFloat SetByCaller;
	SetByCaller.DataTag = GSGASGameplayTags::Data_Damage;
	Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(SetByCaller);
 
	Modifiers.Add(Mod);
}
