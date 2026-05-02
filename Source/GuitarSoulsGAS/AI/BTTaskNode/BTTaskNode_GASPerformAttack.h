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
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
protected:
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
 
	// BP에서 어빌리티 클래스 지정
	UPROPERTY(EditAnywhere, Category = "GAS")
	TSubclassOf<UGameplayAbility> AbilityClass;
 
private:
	void OnAbilityEnded(const FAbilityEndedData& AbilityEndedData);
 
	// 델리게이트 해제용 핸들
	FDelegateHandle AbilityEndedHandle;
 
	// InProgress 중 OwnerComp 참조 (WeakPtr로 안전하게 보관)
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;
};
