// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "GE_GSHeal.generated.h"

/**
 * Health를 SetByCaller(Data.HealAmount) 만큼 즉시 회복
 * GA에서 SetByCallerMagnitude(Data_HealAmount, HealAmount)로 값 주입
 */
UCLASS()
class GUITARSOULSGAS_API UGE_GSHeal : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UGE_GSHeal();
};
