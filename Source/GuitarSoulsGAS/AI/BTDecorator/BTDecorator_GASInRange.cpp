// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator/BTDecorator_GASInRange.h"
#include "AIController.h"
#include "GuitarSoulsGAS.h"
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
		GSGAS_LOG(LogGSGAS, Warning, TEXT("GASInRange: AIController null"));
		return false;
	}

	const APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("GASInRange: ControlledPawn null"));
		return false;
	}

	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("GASInRange: Blackboard null"));
		return false;
	}

	const AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!Target)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("GASInRange: Target null (TargetKey='%s')"), *TargetKey.SelectedKeyName.ToString());
		return false;
	}

	const float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), Target->GetActorLocation());
	const bool bResult = Distance >= MinDistance && Distance <= MaxDistance;
	GSGAS_LOG(LogGSGAS, Log, TEXT("GASInRange: Dist=%.0f Range=[%.0f~%.0f] = %s"),
		Distance, MinDistance, MaxDistance, bResult ? TEXT("IN") : TEXT("OUT"));
	return bResult;
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
	// [진단용 — 확인 후 제거] 이 로그가 뜨지 않으면 TickNode 미호출 → FlowAbortMode/Observer 문제
	GSGAS_LOG(LogGSGAS, Log, TEXT("GASInRange: TickNode called (DeltaSeconds=%.3f)"), DeltaSeconds);

	// 조건이 변경됐을 때만 재평가 요청 (매 틱 RequestExecution은 MoveTo 재시작 버그 유발)
	bool* bLastResult = reinterpret_cast<bool*>(NodeMemory);
	const bool bCurrentResult = CalculateRawConditionValue(OwnerComp, NodeMemory);
	if (bCurrentResult != *bLastResult)
	{
		*bLastResult = bCurrentResult;
		OwnerComp.RequestExecution(this);
	}
}
