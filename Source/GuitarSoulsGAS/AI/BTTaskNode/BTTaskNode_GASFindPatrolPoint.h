// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BTTaskNode_GASFindPatrolPoint.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API UBTTaskNode_GASFindPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTaskNode_GASFindPatrolPoint();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	// 목적지를 기록할 BB 키 (Vector)
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PatrolLocationKey;
};
