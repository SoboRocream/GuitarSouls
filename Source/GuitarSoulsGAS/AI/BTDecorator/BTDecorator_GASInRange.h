// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_GASInRange.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API UBTDecorator_GASInRange : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_GASInRange();
	virtual FString GetStaticDescription() const override;
	
protected:
	UPROPERTY(EditAnywhere, Category="Range")
	float MinDistance = 0.f;

	UPROPERTY(EditAnywhere, Category="Range")
	float MaxDistance = 300.f;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector TargetKey;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
