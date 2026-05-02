// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_GASChance.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API UBTDecorator_GASChance : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_GASChance();
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere, Category="Chance", meta=(ClampMin="0.0", ClampMax="100.0"))
	float ChanceRate = 50.f;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
	
};
