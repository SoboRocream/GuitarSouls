// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attribute/GSAttributeSet.h"
#include "Character/GSGASCharacterBase.h"
#include "Interface/GSGASTargetingInterface.h"
#include "Engine/TargetPoint.h"
#include "GSGASCharacterEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULSGAS_API AGSGASCharacterEnemy : public AGSGASCharacterBase, public IGSGASTargetingInterface
{
	GENERATED_BODY()

public:
	AGSGASCharacterEnemy();
	
	virtual void OnTargeted(bool bTargeted) override;
	
public:
	UPROPERTY(EditAnywhere, Category = "AI|Alert")
	float AlertRadius = 1500.f;

	void NotifyAlert(AActor* InInstigator);
	
	ATargetPoint* GetCurrentPatrolPoint() const;
	void AdvancePatrolIndex();
	
	virtual void PossessedBy(AController* NewController) override;
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOutOfHealth();

	void OnDeath();
	
protected:
	// AttributeSet — ASC 등록은 생성자에서 자동 처리
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<class UGSAttributeSet> AttributeSet;
 
	// 락온 타겟팅 감지용 구체
	// SetCollisionObjectType(CCHANNEL_GSGAS_TARGETING), Visibility Block
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting")
	TObjectPtr<class USphereComponent> TargetingSphere;
 
	// 락온 시 표시할 위젯 컴포넌트 (BP에서 위젯 클래스 지정)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<class UWidgetComponent> LockOnWidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<class UWidgetComponent> HpBarWidgetComponent;
 
	// 초기 스탯 설정 GE (BP에서 지정)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<class UGameplayEffect> InitStatEffectClass;

	// 시작 시 자동 장착할 무기 (지정 시 PossessedBy에서 즉시 전투 모드 진입)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AGSGASWeapon> DefaultWeaponClass;
	
protected:
	UPROPERTY(EditAnywhere, Category = "AI|Patrol")
	TArray<TObjectPtr<ATargetPoint>> PatrolPoints;

	UPROPERTY(VisibleAnywhere, Category = "AI|Patrol")
	int32 PatrolIndex = 0;
};
