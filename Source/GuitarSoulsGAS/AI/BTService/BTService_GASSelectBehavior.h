// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_GASSelectBehavior.generated.h"

/**
 * 매 틱마다 TargetActor 거리를 계산해 BehaviorStateKey(Name)에 기록한다.
 * 값: "Approach" | "Strafe" | "Patrol" | "Idle"
 * BT 에디터에서 표준 Blackboard 데코레이터로 분기한다.
 */
UCLASS()
class GUITARSOULSGAS_API UBTService_GASSelectBehavior : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_GASSelectBehavior();
	virtual FString GetStaticDescription() const override;

	// 전투 유효 거리 — 이 거리 이내면 Strafe, 초과면 Approach
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange = 200.f;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// BB 키 — TargetActor (Object)
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	// BB 키 — 현재 행동 상태 (Name). BT 분기에 사용.
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BehaviorStateKey;
};
