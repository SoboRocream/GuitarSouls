// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Character/GSGASCharacterBase.h"
#include "Interface/GSGASTargetingInterface.h"
#include "GSGASCharacterBoss.generated.h"

/**
 * 보스 캐릭터
 * - CharacterEnemy와 구조 동일하나 순찰/알림 없음
 * - BossHpBar: CreateWidget + AddToViewport (화면 고정형)
 * - HP 임계값 도달 시 GA_BossPhaseTransition 1회 발동
 * - OnDeath는 BlueprintNativeEvent로 노출 (연출 커스터마이징용)
 */
UCLASS()
class GUITARSOULSGAS_API AGSGASCharacterBoss : public AGSGASCharacterBase, public IGSGASTargetingInterface
{
	GENERATED_BODY()

public:
	AGSGASCharacterBoss();

	virtual void OnTargeted(bool bTargeted) override;
	virtual void PossessedBy(AController* NewController) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOutOfHealth();

	void OnHealthChanged(const FOnAttributeChangeData& Data);

	UFUNCTION(BlueprintNativeEvent, Category = "Boss")
	void OnDeath();
	virtual void OnDeath_Implementation();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<class UGSAttributeSet> AttributeSet;

	// 락온 타겟팅 감지용 구체
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting")
	TObjectPtr<class USphereComponent> TargetingSphere;

	// 락온 위젯
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<class UWidgetComponent> LockOnWidgetComponent;

	// 초기 스탯 GE (BP에서 지정)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<class UGameplayEffect> InitStatEffectClass;

	// 시작 시 자동 장착할 무기 (지정 시 PossessedBy에서 즉시 전투 모드 진입)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<class AGSGASWeapon> DefaultWeaponClass;

	// 보스 공격 GA 목록 — StartAbilities(공통)와 분리해 BP 디테일 가독성 확보
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GAS|Attack")
	TArray<TSubclassOf<class UGameplayAbility>> AttackAbilities;

	// 페이즈 전환 GA (BP에서 지정)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phase")
	TSubclassOf<class UGameplayAbility> PhaseTransitionAbilityClass;

	// Phase 2 진입 HP 임계값 (절대값, BP에서 조정)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Phase", meta = (ClampMin = "0.0"))
	float Phase2HPThreshold = 50.f;

	// HpBar 위젯 클래스 (BP에서 지정)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|UI")
	TSubclassOf<class UGSGASBossHpBar> BossHpBarWidgetClass;

	// HpBar에 표시할 보스 이름 (BP에서 지정)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|UI")
	FText BossName = INVTEXT("Boss");

	// 사망 후 Victory 위젯 표시까지의 지연 (초)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|UI")
	float VictoryWidgetDelay = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|UI")
	TSubclassOf<class UGSGASVictoryWidget> VictoryWidgetClass;

private:
	UPROPERTY()
	TObjectPtr<class UGSGASBossHpBar> BossHpBarWidget;

	// 전환이 이미 발동됐는지 여부 (중복 방지 1차 방어)
	bool bPhaseTransitionTriggered = false;

	void ShowVictoryWidget(TSubclassOf<UGSGASVictoryWidget> WidgetClass);
	FTimerHandle VictoryTimerHandle;
};
