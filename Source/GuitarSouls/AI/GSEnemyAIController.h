// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GSEnemyAIController.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULS_API AGSEnemyAIController : public AAIController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UBehaviorTree> BTAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UAIPerceptionComponent> Perception;

	FTimerHandle TimerHandle;

	UPROPERTY()
	TObjectPtr<class AGSTestEnemy> ControlledEnemy;
	
public:
	AGSEnemyAIController();

	void StopUpdateTarget();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

protected:
	void UpdateTarget() const;
	void SetTarget(AActor* InTarget) const;
};
