// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "GSEnumDefine.h"
#include "BTService_SelectBehavior.generated.h"

class AGSTestEnemy;
/**
 * 
 */
UCLASS()
class GUITARSOULS_API UBTService_SelectBehavior : public UBTService
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector BehaviorKey;

	UPROPERTY(EditAnywhere)
	float AttackRangeDistance = 500.f;

public:
	UBTService_SelectBehavior();

protected:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	void SetBehaviorKey(UBlackboardComponent* BlackboardComp, EGSAIBehavior Behavior) const;
	virtual void UpdateBehaviorKey(UBlackboardComponent* BlackboardComp, AGSTestEnemy* ControlledEnemy) const;
};
