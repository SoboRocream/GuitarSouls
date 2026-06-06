// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/BTTaskNode/BTTaskNode_GASPerformAttackPattern.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GuitarSoulsGAS.h"

UBTTaskNode_GASPerformAttackPattern::UBTTaskNode_GASPerformAttackPattern()
{
	NodeName = TEXT("GAS Perform Attack Pattern");

	// AI 인스턴스마다 독립된 ShuffleBag 상태를 갖기 위해 인스턴스 생성 활성화
	bCreateNodeInstance = true;
	bNotifyTaskFinished = true;
}

FString UBTTaskNode_GASPerformAttackPattern::GetStaticDescription() const
{
	if (Patterns.IsEmpty())
	{
		return TEXT("Patterns: (없음)");
	}

	TArray<FString> Names;
	for (const FGASAttackPatternEntry& Entry : Patterns)
	{
		Names.Add(Entry.AbilityClass ? Entry.AbilityClass->GetName() : TEXT("None"));
	}
	return FString::Printf(TEXT("ShuffleBag [%d]\n%s"), Patterns.Num(), *FString::Join(Names, TEXT("\n")));
}

EBTNodeResult::Type UBTTaskNode_GASPerformAttackPattern::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (Patterns.IsEmpty())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BTTaskNode_GASPerformAttackPattern: Patterns is empty."));
		return EBTNodeResult::Failed;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BTTaskNode_GASPerformAttackPattern: AIController is null."));
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AIController->GetPawn();
	if (!Pawn)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BTTaskNode_GASPerformAttackPattern: Pawn is null."));
		return EBTNodeResult::Failed;
	}

	IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(Pawn);
	if (!ASCInterface)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BTTaskNode_GASPerformAttackPattern: Pawn does not implement IAbilitySystemInterface."));
		return EBTNodeResult::Failed;
	}

	UAbilitySystemComponent* ASC = ASCInterface->GetAbilitySystemComponent();
	if (!ASC)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BTTaskNode_GASPerformAttackPattern: ASC is null."));
		return EBTNodeResult::Failed;
	}

	const int32 PatternIndex = PickNextPatternIndex();
	const FGASAttackPatternEntry& Entry = Patterns[PatternIndex];

	if (!Entry.AbilityClass)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BTTaskNode_GASPerformAttackPattern: AbilityClass at index [%d] is not set."), PatternIndex);
		return EBTNodeResult::Failed;
	}

	CurrentAbilityClass = Entry.AbilityClass;

	bool bActivated = false;
	if (Entry.EventTag.IsValid())
	{
		const FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(Entry.AbilityClass);
		GSGAS_LOG(LogGSGAS, Log, TEXT("[Diag] PatternIndex: %d | AbilityClass: %s | Granted: %s | EventTag: %s"),
			PatternIndex,
			*Entry.AbilityClass->GetName(),
			Spec ? TEXT("YES") : TEXT("NO — 어빌리티가 Grant되지 않음"),
			*Entry.EventTag.ToString());

		if (Spec)
		{
			FGameplayEventData EventData;
			EventData.EventTag = Entry.EventTag;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Pawn, Entry.EventTag, EventData);

			const FGameplayAbilitySpec* SpecAfter = ASC->FindAbilitySpecFromClass(Entry.AbilityClass);
			bActivated = SpecAfter && SpecAfter->IsActive();

			GSGAS_LOG(LogGSGAS, Log, TEXT("[Diag] SendGameplayEvent 후 IsActive: %s | %s"),
				bActivated ? TEXT("YES") : TEXT("NO"),
				bActivated ? TEXT("정상")
				           : TEXT("AbilityTriggers에 EventTag 미등록 의심 — BP 클래스 디폴트에서 Ability Triggers 항목에 해당 태그 추가 필요"));
		}
	}
	else
	{
		const FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(Entry.AbilityClass);
		GSGAS_LOG(LogGSGAS, Log, TEXT("[Diag] TryActivateAbilityByClass | AbilityClass: %s | Granted: %s"),
			*Entry.AbilityClass->GetName(),
			Spec ? TEXT("YES") : TEXT("NO — StartAbilities에 등록됐는지 확인 필요"));

		bActivated = ASC->TryActivateAbilityByClass(Entry.AbilityClass);
		GSGAS_LOG(LogGSGAS, Log, TEXT("[Diag] TryActivateAbilityByClass result: %s"),
			bActivated ? TEXT("activated") : TEXT("FAILED — CanActivateAbility 실패 (BlockedTags/RequiredTags/Cost 확인)"));
	}

	if (!bActivated)
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BTTaskNode_GASPerformAttackPattern: Activation failed for [%s] (index %d)."),
			*Entry.AbilityClass->GetName(), PatternIndex);
		return EBTNodeResult::Failed;
	}

	CachedOwnerComp = &OwnerComp;
	CachedASC       = ASC;

	AbilityEndedHandle = ASC->OnAbilityEnded.AddUObject(this, &UBTTaskNode_GASPerformAttackPattern::OnAbilityEnded);

	GSGAS_LOG(LogGSGAS, Log, TEXT("BTTaskNode_GASPerformAttackPattern: [%s] (index %d) activated. Waiting for end."),
		*Entry.AbilityClass->GetName(), PatternIndex);

	return EBTNodeResult::InProgress;
}

void UBTTaskNode_GASPerformAttackPattern::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	if (AbilityEndedHandle.IsValid() && CachedASC.IsValid())
	{
		CachedASC->OnAbilityEnded.Remove(AbilityEndedHandle);
		AbilityEndedHandle.Reset();
	}

	CachedOwnerComp.Reset();
	CachedASC.Reset();
	CurrentAbilityClass = nullptr;
}

// ── Shuffle Bag ──────────────────────────────────────────────

void UBTTaskNode_GASPerformAttackPattern::RefillShuffleBag()
{
	ShuffleBag.Reset(Patterns.Num());
	for (int32 i = 0; i < Patterns.Num(); ++i)
	{
		ShuffleBag.Add(i);
	}
	GSGAS_LOG(LogGSGAS, Log, TEXT("BTTaskNode_GASPerformAttackPattern: ShuffleBag refilled (%d patterns)."), ShuffleBag.Num());
}

int32 UBTTaskNode_GASPerformAttackPattern::PickNextPatternIndex()
{
	if (ShuffleBag.IsEmpty())
	{
		RefillShuffleBag();
	}

	const int32 RandIdx = FMath::RandRange(0, ShuffleBag.Num() - 1);
	const int32 PickedPatternIdx = ShuffleBag[RandIdx];
	ShuffleBag.RemoveAtSwap(RandIdx);

	GSGAS_LOG(LogGSGAS, Log, TEXT("BTTaskNode_GASPerformAttackPattern: Picked pattern index [%d]. Bag remaining: %d"),
		PickedPatternIdx, ShuffleBag.Num());

	return PickedPatternIdx;
}

// ── Ability End Callback ──────────────────────────────────────

void UBTTaskNode_GASPerformAttackPattern::OnAbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	if (!AbilityEndedData.AbilityThatEnded || AbilityEndedData.AbilityThatEnded->GetClass() != CurrentAbilityClass)
	{
		return;
	}

	if (!CachedOwnerComp.IsValid())
	{
		GSGAS_LOG(LogGSGAS, Warning, TEXT("BTTaskNode_GASPerformAttackPattern: CachedOwnerComp is invalid on ability end."));
		return;
	}

	if (CachedASC.IsValid())
	{
		CachedASC->OnAbilityEnded.Remove(AbilityEndedHandle);
	}
	AbilityEndedHandle.Reset();

	const EBTNodeResult::Type Result = AbilityEndedData.bWasCancelled ? EBTNodeResult::Failed : EBTNodeResult::Succeeded;
	GSGAS_LOG(LogGSGAS, Log, TEXT("BTTaskNode_GASPerformAttackPattern: Ability ended. Result=%s"),
		Result == EBTNodeResult::Succeeded ? TEXT("Succeeded") : TEXT("Failed"));

	FinishLatentTask(*CachedOwnerComp.Get(), Result);
}
