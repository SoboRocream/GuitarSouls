// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTaskNode/BTTaskNode_GASFindPatrolPoint.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Character/GSGASCharacterEnemy.h"
#include "Engine/TargetPoint.h"
#include "GuitarSoulsGAS.h"

UBTTaskNode_GASFindPatrolPoint::UBTTaskNode_GASFindPatrolPoint()
{
	NodeName = TEXT("GAS Find Patrol Point");
}

EBTNodeResult::Type UBTTaskNode_GASFindPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BTTaskNode_GASFindPatrolPoint: AIController is null."));
		return EBTNodeResult::Failed;
	}
 
	AGSGASCharacterEnemy* Enemy = Cast<AGSGASCharacterEnemy>(AIController->GetPawn());
	if (!Enemy)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BTTaskNode_GASFindPatrolPoint: Pawn is not AGSGASCharacterEnemy."));
		return EBTNodeResult::Failed;
	}
 
	ATargetPoint* PatrolPoint = Enemy->GetCurrentPatrolPoint();
	if (!PatrolPoint)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BTTaskNode_GASFindPatrolPoint: No patrol point available on %s."), *Enemy->GetName());
		return EBTNodeResult::Failed;
	}
 
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BTTaskNode_GASFindPatrolPoint: BlackboardComponent is null."));
		return EBTNodeResult::Failed;
	}
 
	BB->SetValueAsVector(PatrolLocationKey.SelectedKeyName, PatrolPoint->GetActorLocation());
 
	Enemy->AdvancePatrolIndex();
 
	GSGAS_LOG(LogGSGAS, Log, TEXT("BTTaskNode_GASFindPatrolPoint: Patrol target set to %s."), *PatrolPoint->GetName());
	return EBTNodeResult::Succeeded;
}
