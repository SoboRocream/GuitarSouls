// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_GSGASUpdateLastKnown.generated.h"

/**
 * TargetActor가 유효한 동안 매 틱 LastKnownLocation BB 키를 갱신한다.
 * TargetActor를 잃은 후에도 마지막 위치가 보존되어 ReturnToLastKnown Task에서 사용된다.
 */
UCLASS()
class GUITARSOULSGAS_API UBTService_GSGASUpdateLastKnown : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_GSGASUpdateLastKnown();
	virtual FString GetStaticDescription() const override;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// BB 키 — 추적 대상 (Object / AActor)
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	// BB 키 — 마지막으로 확인된 위치 (Vector)
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LastKnownLocationKey;
};
