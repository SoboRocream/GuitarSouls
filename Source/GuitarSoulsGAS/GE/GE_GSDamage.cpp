// Fill out your copyright notice in the Description page of Project Settings.


#include "GE/GE_GSDamage.h"
#include "Attribute/GSAttributeSet.h"
#include "GameplayEffectComponents/TargetTagRequirementsGameplayEffectComponent.h"
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

	// 무적(롤) 또는 사망 상태인 대상에게는 데미지 GE 적용 안 함
	// UE5.3+: AddComponent는 생성자 밖 전용. 생성자 안에서는 CreateDefaultSubobject 사용
	UTargetTagRequirementsGameplayEffectComponent* TagReqComp =
		CreateDefaultSubobject<UTargetTagRequirementsGameplayEffectComponent>(TEXT("TargetTagRequirements"));
	TagReqComp->ApplicationTagRequirements.IgnoreTags.AddTag(GSGASGameplayTags::Character_State_Invincible);
	TagReqComp->ApplicationTagRequirements.IgnoreTags.AddTag(GSGASGameplayTags::Character_State_Death);
	GEComponents.Add(TagReqComp);
}
