// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_GASPerformAttackPattern.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;

/**
 * 공격 패턴 하나의 데이터
 * - AbilityClass: 발동할 GA
 * - EventTag: SendGameplayEvent 방식 사용 시 태그 (GA_BossAttack용)
 */
USTRUCT(BlueprintType)
struct FGASAttackPatternEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayAbility> AbilityClass;

	// 설정 시 SendGameplayEventToActor로 활성화
	// 미설정 시 TryActivateAbilityByClass 사용
	UPROPERTY(EditAnywhere, Category = "GAS")
	FGameplayTag EventTag;
};

/**
 * Shuffle Bag 방식 랜덤 패턴 선택 공격 Task
 *
 * 동작:
 *   - Patterns 배열에 등록된 GA를 Shuffle Bag으로 관리
 *   - 매 실행 시 Bag에서 하나를 뽑아 활성화
 *   - Bag이 비면 전체 패턴을 다시 채우고 셔플 (모든 패턴 1회 보장)
 *   - GA 종료 시 Succeeded / Cancelled 시 Failed 반환
 *
 * 주의:
 *   - bCreateNodeInstance = true → AI 인스턴스마다 독립된 Bag 상태 유지
 *   - Patterns가 비어있으면 즉시 Failed 반환
 */
UCLASS()
class GUITARSOULSGAS_API UBTTaskNode_GASPerformAttackPattern : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskNode_GASPerformAttackPattern();

	virtual FString GetStaticDescription() const override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	// BP에서 패턴 목록 지정
	UPROPERTY(EditAnywhere, Category = "GAS|Pattern")
	TArray<FGASAttackPatternEntry> Patterns;

private:
	// Bag이 비면 전체 인덱스로 채움
	void RefillShuffleBag();

	// Bag에서 다음 패턴 인덱스를 뽑음 (비어있으면 자동 Refill)
	int32 PickNextPatternIndex();

	void OnAbilityEnded(const FAbilityEndedData& AbilityEndedData);

	// Shuffle Bag 상태 — bCreateNodeInstance=true 이므로 멤버로 관리
	TArray<int32> ShuffleBag;

	// OnAbilityEnded 식별용 (매 실행마다 갱신)
	TSubclassOf<UGameplayAbility> CurrentAbilityClass;

	FDelegateHandle AbilityEndedHandle;
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;
};
