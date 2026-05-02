// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator/BTDecorator_GASInRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_GASInRange::UBTDecorator_GASInRange()
{
	NodeName = TEXT("GAS In Range");
	TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_GASInRange, TargetKey), AActor::StaticClass());
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
