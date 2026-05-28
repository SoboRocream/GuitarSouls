// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_GSPotionCost.generated.h"

/**
 * PotionCount를 1 감소 (고정값)
 * GA_UsePotion의 CostGameplayEffectClass로 사용
 */
UCLASS()
class GUITARSOULSGAS_API UGE_GSPotionCost : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UGE_GSPotionCost();
};
