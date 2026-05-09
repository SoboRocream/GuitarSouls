// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator/BTDecorator_GASInRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_GASInRange::UBTDecorator_GASInRange()
{
	NodeName = TEXT("GAS In Range");
	TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_GASInRange, TargetKey), AActor::StaticClass());
	bNotifyTick = true;
}

FString UBTDecorator_GASInRange::GetStaticDescription() const
{
	return FString::Printf(TEXT("Distance: %.1f ~ %.1f"), MinDistance, MaxDistance);
}

bool UBTDecorator_GASInRange::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return false;
	}

	const APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return false;
	}

	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
	{
		return false;
	}

	const AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!Target)
	{
		return false;
	}

	const float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), Target->GetActorLocation());
	return Distance >= MinDistance && Distance <= MaxDistance;
}

void UBTDecorator_GASInRange::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	// 초기 상태 기록
	bool* bLastResult = reinterpret_cast<bool*>(NodeMemory);
	*bLastResult = CalculateRawConditionValue(OwnerComp, NodeMemory);
}

void UBTDecorator_GASInRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// 조건이 변경됐을 때만 재평가 요청 (매 틱 RequestExecution은 MoveTo 재시작 버그 유발)
	bool* bLastResult = reinterpret_cast<bool*>(NodeMemory);
	const bool bCurrentResult = CalculateRawConditionValue(OwnerComp, NodeMemory);
	if (bCurrentResult != *bLastResult)
	{
		*bLastResult = bCurrentResult;
		OwnerComp.RequestExecution(this);
	}
}
