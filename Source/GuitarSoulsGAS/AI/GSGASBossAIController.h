// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "GSGASBossAIController.generated.h"

/**
 * 보스 AI 컨트롤러
 * - Perception 없음 — OnPossess 시점에 플레이어를 즉시 타겟으로 설정
 * - 플레이어 Death 태그 구독으로 사망 감지 → 타겟 해제
 */
UCLASS()
class GUITARSOULSGAS_API AGSGASBossAIController : public AAIController
{
	GENERATED_BODY()

public:
	AGSGASBossAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	void SetTarget(AActor* InTarget);
	void OnPlayerDeathTagChanged(const FGameplayTag Tag, int32 NewCount);

	// OnPossess 직후 플레이어가 아직 없을 경우를 대비한 지연 탐색
	void TryFindAndSetTarget();

protected:
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<class UBehaviorTree> BTAsset;

private:
	TWeakObjectPtr<class AGSGASCharacterBoss> ControlledBoss;

	// 플레이어 Death 태그 구독 해제용 핸들
	FDelegateHandle PlayerDeathTagHandle;
};
