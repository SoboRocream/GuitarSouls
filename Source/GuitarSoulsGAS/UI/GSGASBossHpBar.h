// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "UI/GSGASUserWidget.h"
#include "GSGASBossHpBar.generated.h"

/**
 * 보스 전용 HpBar 위젯
 *
 * 표시 조건:
 *   - AI.State.Combat 태그 부여 시 (보스전 시작)
 *   - HP 감소 시 (피격)
 *
 * UMG에서 반드시 동일한 이름으로 위젯 바인딩 필요:
 *   - HealthBarWidget (GSGASBarWidget)
 *   - BossNameText   (TextBlock)
 */
UCLASS()
class GUITARSOULSGAS_API UGSGASBossHpBar : public UGSGASUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetAbilitySystemComponent(AActor* InOwner) override;

	// BP에서 보스 이름 설정용
	UFUNCTION(BlueprintCallable, Category = "Boss|UI")
	void SetBossName(const FText& InName);

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UGSGASBarWidget> HealthBarWidget;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> BossNameText;

private:
	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnCombatTagChanged(const FGameplayTag Tag, int32 NewCount);

	FDelegateHandle CombatTagHandle;
};
