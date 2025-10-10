// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/BTService_SelectBehavior.h"
#include "BTService_SelectBehaviorBoss.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULS_API UBTService_SelectBehaviorBoss : public UBTService_SelectBehavior
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	float StaminaCheckValue = 40.f;

protected:
	virtual void UpdateBehaviorKey(UBlackboardComponent* BlackboardComp, AGSTestEnemy* ControlledEnemy) const override;
};
