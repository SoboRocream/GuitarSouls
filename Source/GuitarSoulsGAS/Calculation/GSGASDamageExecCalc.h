// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GSGASDamageExecCalc.generated.h"

/**
 * 데미지 산출 ExecutionCalculation.
 * 최종 데미지 = 기본 데미지(SetByCaller Data.Damage) × 공격자 AttackPower − 대상 Defense (하한 0)
 * 기본 데미지는 각 공격 GA가 기존처럼 SetByCaller로 주입하므로, GA 코드 변경이 없다.
 */
UCLASS()
class GUITARSOULSGAS_API UGSGASDamageExecCalc : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UGSGASDamageExecCalc();

	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
