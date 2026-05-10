// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "AITypes.h"
#include "BTTaskNode_GASStrafe.generated.h"

/**
 * 타겟 주변 측면 이동 Task
 * - StrafeDirectionKey(BB Name: "Left"/"Right")로 방향을 유지하며 DirectionFlipChance 확률로 전환한다.
 * - NavMesh 투영 실패 시 반대 방향을 재시도하고, 양방향 모두 실패하면 Task Failed를 반환한다.
 * - 이동 완료는 TickTask에서 PathFollowingComponent 요청 ID 비교로 감지한다.
 */
UCLASS()
class GUITARSOULSGAS_API UBTTaskNode_GASStrafe : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskNode_GASStrafe();
	virtual FString GetStaticDescription() const override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	// BB 키 — 추적 대상 (Object / AActor)
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	// BB 키 — Strafe 방향 유지 (Name: "Left" / "Right")
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector StrafeDirectionKey;

	// 타겟으로부터 유지할 전투 거리 (cm)
	UPROPERTY(EditAnywhere, Category = "Strafe", meta = (ClampMin = "50.0"))
	float PreferredRadius = 400.f;

	// 거리 허용 오차: PreferredRadius ± Tolerance 범위 내 목적지 산출 (cm)
	// 너무 크면 최소 횡이동량이 줄어드므로 작게 유지
	UPROPERTY(EditAnywhere, Category = "Strafe", meta = (ClampMin = "0.0"))
	float RadiusTolerance = 40.f;

	// 한 번 이동할 횡이동 각도 (도) — 클수록 1스텝 이동 거리가 길어짐
	UPROPERTY(EditAnywhere, Category = "Strafe", meta = (ClampMin = "15.0", ClampMax = "120.0"))
	float StrafeAngleDeg = 70.f;

	// 매 실행마다 방향을 전환할 확률 (0.0 ~ 1.0)
	UPROPERTY(EditAnywhere, Category = "Strafe", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DirectionFlipChance = 0.3f;

	// Task 한 번 실행에서 연속으로 이동할 스텝 수 (값이 클수록 길고 연속적인 횡이동)
	UPROPERTY(EditAnywhere, Category = "Strafe", meta = (ClampMin = "1", ClampMax = "6"))
	int32 StrafeStepCount = 3;

private:
	bool TryMoveToStrafePoint(UBehaviorTreeComponent& OwnerComp, const FVector& PawnLoc, const FVector& TargetLoc, bool bGoLeft);
	FVector ComputeStrafeDestination(const FVector& PawnLoc, const FVector& TargetLoc, bool bGoLeft) const;

	FAIRequestID MoveRequestID;
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
	TWeakObjectPtr<AAIController> CachedAIController;

	// PFC가 MoveRequestID를 실제로 처리 중임을 확인한 후에만 완료 감지
	bool bMoveStarted = false;

	// 남은 연속 스텝 수
	int32 RemainingSteps = 0;

	// 현재 진행 중인 방향 (스텝 간 유지)
	bool bCurrentGoLeft = true;
};
