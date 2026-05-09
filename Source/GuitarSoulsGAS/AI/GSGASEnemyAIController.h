// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "GSGASEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API AGSGASEnemyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AGSGASEnemyAIController();
 
	// AGSGASCharacterEnemy::NotifyAlert에서 호출
	void SetAlertTarget(AActor* InInstigator);
 
protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
 
private:
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
 
	void SetTarget(AActor* InTarget);
	void PropagateAlert(AActor* InInstigator);
 
protected:
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<class UBehaviorTree> BTAsset;

	// 시야를 잃어도 타겟을 유지하는 근접 거리 (이 거리 이내면 Lose Sight 무시)
	UPROPERTY(EditAnywhere, Category = "AI", meta = (ClampMin = "0.0"))
	float KeepTargetDistance = 300.f;
 
	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<class UAIPerceptionComponent> Perception;
 
private:
	TWeakObjectPtr<class AGSGASCharacterEnemy> ControlledEnemy;
};
