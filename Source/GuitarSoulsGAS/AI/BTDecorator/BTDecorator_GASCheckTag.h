// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_GASCheckTag.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API UBTDecorator_GASCheckTag : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UBTDecorator_GASCheckTag();
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere, Category="GameplayTag")
	FGameplayTag RequiredTag;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
