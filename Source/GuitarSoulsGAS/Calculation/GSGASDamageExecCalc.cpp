// Fill out your copyright notice in the Description page of Project Settings.


#include "Calculation/GSGASDamageExecCalc.h"

#include "Attribute/GSAttributeSet.h"
#include "GuitarSoulsGAS.h"
#include "Tags/GSGASGameplayTags.h"

// 캡처할 어트리뷰트 정의: 공격자 AttackPower(스냅샷), 대상 Defense(비스냅샷)
// 어트리뷰트 프로퍼티가 protected이므로, public 정적 게터로 캡처 정의를 직접 구성한다.
struct FGSGASDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition AttackPowerDef;
	FGameplayEffectAttributeCaptureDefinition DefenseDef;

	FGSGASDamageStatics()
	{
		AttackPowerDef = FGameplayEffectAttributeCaptureDefinition(
			UGSAttributeSet::GetAttackPowerAttribute(),
			EGameplayEffectAttributeCaptureSource::Source, /*bSnapshot=*/true);

		DefenseDef = FGameplayEffectAttributeCaptureDefinition(
			UGSAttributeSet::GetDefenseAttribute(),
			EGameplayEffectAttributeCaptureSource::Target, /*bSnapshot=*/false);
	}
};

static const FGSGASDamageStatics& DamageStatics()
{
	static FGSGASDamageStatics Statics;
	return Statics;
}

UGSGASDamageExecCalc::UGSGASDamageExecCalc()
{
	RelevantAttributesToCapture.Add(DamageStatics().AttackPowerDef);
	RelevantAttributesToCapture.Add(DamageStatics().DefenseDef);
}

void UGSGASDamageExecCalc::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// 기본 데미지: 공격 GA가 SetByCaller(Data.Damage)로 주입한 무기 데미지 (기존 계약 유지)
	const float BaseDamage = Spec.GetSetByCallerMagnitude(GSGASGameplayTags::Data_Damage, false, 0.f);

	float AttackPower = 1.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackPowerDef, EvalParams, AttackPower);
	if (AttackPower <= 0.f)
	{
		// 방어적 처리: 잘못된 값이 들어와도 원본 데미지를 보존
		AttackPower = 1.f;
	}

	float Defense = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().DefenseDef, EvalParams, Defense);
	Defense = FMath::Max(0.f, Defense);

	// 최종 데미지 = 기본 × 공격력 − 방어력, 하한 0
	const float FinalDamage = FMath::Max(BaseDamage * AttackPower - Defense, 0.f);

	if (FinalDamage > 0.f)
	{
		// Damage 메타어트리뷰트에 출력 → 기존 PostGameplayEffectExecute가 Health로 반영
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
			UGSAttributeSet::GetDamageAttribute(), EGameplayModOp::Additive, FinalDamage));
	}
}
