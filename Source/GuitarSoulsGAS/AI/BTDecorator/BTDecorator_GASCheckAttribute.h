// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "GameplayEffectTypes.h"
#include "BTDecorator_GASCheckAttribute.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API UBTDecorator_GASCheckAttribute : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_GASCheckAttribute();
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere, Category="Attribute")
	FGameplayAttribute Attribute;

	UPROPERTY(EditAnywhere, Category="Attribute")
	float CompareValue = 0.f;

	UPROPERTY(EditAnywhere, Category="Attribute")
	bool bCheckLessThan = true;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
