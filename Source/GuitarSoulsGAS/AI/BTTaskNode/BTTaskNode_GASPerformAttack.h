// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_GASPerformAttack.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API UBTTaskNode_GASPerformAttack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTaskNode_GASPerformAttack();
	virtual FString GetStaticDescription() const override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
protected:
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
 
	// BP에서 어빌리티 클래스 지정
	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayAbility> AbilityClass;

	// 설정 시 SendGameplayEventToActor로 활성화 (GA_EnemyAttack용)
	// 미설정 시 TryActivateAbilityByClass 사용 (기존 방식)
	// ※ 어빌리티 BP의 AbilityTriggers에 동일 태그가 등록되어 있어야 함
	UPROPERTY(EditAnywhere, Category = "GAS")
	FGameplayTag EventTag;

	// EventTag 사용 시 GA_EnemyAttack에 전달할 콤보 수 (Light: 1~3, Heavy: 1)
	UPROPERTY(EditAnywhere, Category = "GAS", meta = (ClampMin = "1", ClampMax = "3", EditCondition = "EventTag.IsValid()"))
	float EventMagnitude = 1.f;
 
private:
	void OnAbilityEnded(const FAbilityEndedData& AbilityEndedData);
 
	// 델리게이트 해제용 핸들
	FDelegateHandle AbilityEndedHandle;
 
	// InProgress 중 OwnerComp 참조 (WeakPtr로 안전하게 보관)
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;
};
