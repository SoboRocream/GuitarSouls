// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTaskNode/BTTaskNode_GASPerformAttack.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GuitarSoulsGAS.h"

UBTTaskNode_GASPerformAttack::UBTTaskNode_GASPerformAttack()
{
	NodeName = TEXT("GAS Perform Attack");
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTaskNode_GASPerformAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!AbilityClass)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BTTaskNode_GASPerformAttack: AbilityClass is not set."));
		return EBTNodeResult::Failed;
	}
 
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BTTaskNode_GASPerformAttack: AIController is null."));
		return EBTNodeResult::Failed;
	}
 
	APawn* Pawn = AIController->GetPawn();
	if (!Pawn)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BTTaskNode_GASPerformAttack: Pawn is null."));
		return EBTNodeResult::Failed;
	}
 
	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Pawn);
	if (!ASCInterface)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BTTaskNode_GASPerformAttack: Pawn does not implement IAbilitySystemInterface."));
		return EBTNodeResult::Failed;
	}
 
	UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
	if (!ASC)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BTTaskNode_GASPerformAttack: ASC is null."));
		return EBTNodeResult::Failed;
	}
 
	bool bActivated = ASC->TryActivateAbilityByClass(AbilityClass);
	if (!bActivated)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BTTaskNode_GASPerformAttack: TryActivateAbilityByClass failed for [%s]."), *AbilityClass->GetName());
		return EBTNodeResult::Failed;
	}
 
	CachedOwnerComp = &OwnerComp;
	CachedASC = ASC;
 
	AbilityEndedHandle = ASC->OnAbilityEnded.AddUObject(this, &UBTTaskNode_GASPerformAttack::OnAbilityEnded);
 
	GSGAS_LOG(LogGSGAS, Log, TEXT("BTTaskNode_GASPerformAttack: Ability [%s] activated. Waiting for end."), *AbilityClass->GetName());
	return EBTNodeResult::InProgress;
}

void UBTTaskNode_GASPerformAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	// Task가 외부에서 중단될 경우(Abort 등) 델리게이트 정리
	if (AbilityEndedHandle.IsValid() && CachedASC.IsValid())
	{
		CachedASC->OnAbilityEnded.Remove(AbilityEndedHandle);
		AbilityEndedHandle.Reset();
	}
 
	CachedOwnerComp.Reset();
	CachedASC.Reset();
}

void UBTTaskNode_GASPerformAttack::OnAbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	// 이 Task가 활성화한 AbilityClass 종료인지 확인
	if (!AbilityEndedData.AbilityThatEnded || AbilityEndedData.AbilityThatEnded->GetClass() != AbilityClass)
	{
		return;
	}
 
	if (!CachedOwnerComp.IsValid())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BTTaskNode_GASPerformAttack: CachedOwnerComp is invalid on ability end."));
		return;
	}
 
	// 델리게이트 해제
	if (CachedASC.IsValid())
	{
		CachedASC->OnAbilityEnded.Remove(AbilityEndedHandle);
	}
	AbilityEndedHandle.Reset();
 
	EBTNodeResult::Type Result = AbilityEndedData.bWasCancelled ? EBTNodeResult::Failed : EBTNodeResult::Succeeded;
	GSGAS_LOG(LogGSGAS, Log, TEXT("BTTaskNode_GASPerformAttack: Ability ended. Result=%s"), Result == EBTNodeResult::Succeeded ? TEXT("Succeeded") : TEXT("Failed"));
 
	FinishLatentTask(*CachedOwnerComp.Get(), Result);
}
