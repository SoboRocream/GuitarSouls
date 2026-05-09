// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTaskNode/BTTaskNode_GASPerformAttack.h"
#include "AbilitySystemBlueprintLibrary.h"
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

FString UBTTaskNode_GASPerformAttack::GetStaticDescription() const
{
	const FString ClassName = AbilityClass ? AbilityClass->GetName() : TEXT("None");
	if (EventTag.IsValid())
	{
		return FString::Printf(TEXT("Perform: %s | Event: %s (x%.0f)"),
			*ClassName, *EventTag.GetTagName().ToString(), EventMagnitude);
	}
	return FString::Printf(TEXT("Perform: %s"), *ClassName);
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
 
	// EventTag가 설정된 경우: SendGameplayEventToActor (GA_EnemyAttack 전용)
	// 미설정인 경우: TryActivateAbilityByClass (기존 방식)
	bool bActivated = false;
	if (EventTag.IsValid())
	{
		const FGameplayAbilitySpec* SpecBefore = ASC->FindAbilitySpecFromClass(AbilityClass);
		GSGAS_LOG(LogGSGAS, Log, TEXT("[Diag] AbilityClass: %s | Granted: %s | EventTag: %s"),
			*AbilityClass->GetName(),
			SpecBefore ? TEXT("YES") : TEXT("NO — 어빌리티가 Grant되지 않음"),
			*EventTag.ToString());

		if (SpecBefore)
		{
			FGameplayEventData EventData;
			EventData.EventTag = EventTag;
			EventData.EventMagnitude = EventMagnitude;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Pawn, EventTag, EventData);

			const FGameplayAbilitySpec* SpecAfter = ASC->FindAbilitySpecFromClass(AbilityClass);
			bActivated = SpecAfter && SpecAfter->IsActive();
			GSGAS_LOG(LogGSGAS, Log, TEXT("[Diag] SendGameplayEvent 후 IsActive: %s | %s"),
				bActivated ? TEXT("YES") : TEXT("NO"),
				bActivated ? TEXT("정상") : TEXT("AbilityTriggers에 EventTag 미등록 의심"));
		}
	}
	else
	{
		bActivated = ASC->TryActivateAbilityByClass(AbilityClass);
	}

	if (!bActivated)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BTTaskNode_GASPerformAttack: Activation failed for [%s]."), *AbilityClass->GetName());
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
