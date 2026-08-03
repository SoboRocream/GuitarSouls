// Fill out your copyright notice in the Description page of Project Settings.


#include "GE/GE_GSDamage.h"
#include "Attribute/GSAttributeSet.h"
#include "Calculation/GSGASDamageExecCalc.h"
#include "GameplayEffectComponents/TargetTagRequirementsGameplayEffectComponent.h"
#include "Tags/GSGASGameplayTags.h"

UGE_GSDamage::UGE_GSDamage()
{
	DurationPolicy = EGameplayEffectDurationType::Instant;

	// 최종 데미지 산출은 DamageExecCalc에서 처리한다.
	// 기본 데미지는 GA가 기존처럼 SetByCaller(Data.Damage)로 주입 →
	// ExecCalc가 공격자 AttackPower / 대상 Defense를 반영해 Damage 어트리뷰트로 출력.
	FGameplayEffectExecutionDefinition ExecDef;
	ExecDef.CalculationClass = UGSGASDamageExecCalc::StaticClass();
	Executions.Add(ExecDef);

	// 무적(롤) 또는 사망 상태인 대상에게는 데미지 GE 적용 안 함
	// UE5.3+: AddComponent는 생성자 밖 전용. 생성자 안에서는 CreateDefaultSubobject 사용
	UTargetTagRequirementsGameplayEffectComponent* TagReqComp =
		CreateDefaultSubobject<UTargetTagRequirementsGameplayEffectComponent>(TEXT("TargetTagRequirements"));
	TagReqComp->ApplicationTagRequirements.IgnoreTags.AddTag(GSGASGameplayTags::Character_State_Invincible);
	TagReqComp->ApplicationTagRequirements.IgnoreTags.AddTag(GSGASGameplayTags::Character_State_Death);
	GEComponents.Add(TagReqComp);
}
