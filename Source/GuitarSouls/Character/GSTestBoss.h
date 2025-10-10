// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GSTestEnemy.h"
#include "GSTestBoss.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULS_API AGSTestBoss : public AGSTestEnemy
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UGSUserWidget> BossHealthBarWidgetClass;

	UPROPERTY()
	TObjectPtr<class UGSBossHpBar> BossHpBarWidget;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundWave>  BossMusicAsset;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UAudioComponent>  BossMusic;

	bool bStartedBossMusic = false;

public:
	virtual void WatchTarget(AActor* InTargetActor) override;
	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void OnDeath() override;
	virtual void HitReaction(const AActor* Attacker) override;
	
};
