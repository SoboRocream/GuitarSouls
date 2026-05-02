// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "AbilitySystemComponent.h"
#include "BTService_GASSelectBehavior.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API UBTService_GASSelectBehavior : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_GASSelectBehavior();
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
 
	// 전투 유효 거리 — 이 거리 이내면 Strafe, 초과면 Approach
	UPROPERTY(EditAnywhere, Category = "AI")
	float AttackRange = 200.f;
 
	// BB 키 — TargetActor (Object)
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
 
private:
	void SetBehaviorTag(UAbilitySystemComponent* ASC, const FGameplayTag& NewTag);
 
	FGameplayTag CurrentBehaviorTag;
	
};
