// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/GSGASBossHpBar.h"

#include "AbilitySystemComponent.h"
#include "Components/TextBlock.h"
#include "GuitarSoulsGAS.h"
#include "UI/GSGASBarWidget.h"
#include "Attribute/GSAttributeSet.h"
#include "Tags/GSGASGameplayTags.h"

void UGSGASBossHpBar::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);

	UAbilitySystemComponent* InASC = GetAbilitySystemComponent();
	if (!InASC) return;

	const UGSAttributeSet* AttributeSet = InASC->GetSet<UGSAttributeSet>();
	if (!AttributeSet) return;

	// HP 변화 구독
	InASC->GetGameplayAttributeValueChangeDelegate(UGSAttributeSet::GetHealthAttribute())
		.AddUObject(this, &UGSGASBossHpBar::OnHealthChanged);

	// 전투 시작 태그 구독
	CombatTagHandle = InASC->RegisterGameplayTagEvent(
		GSGASGameplayTags::AI_State_Combat,
		EGameplayTagEventType::NewOrRemoved)
		.AddUObject(this, &UGSGASBossHpBar::OnCombatTagChanged);

	// 초기 비율 설정 (PossessedBy 이후 호출되므로 수치 보장됨)
	if (HealthBarWidget)
	{
		const float MaxHealth = AttributeSet->GetMaxHealth();
		HealthBarWidget->SetRatio(MaxHealth > 0.f ? AttributeSet->GetHealth() / MaxHealth : 0.f);
	}

	// 초기 상태는 숨김 — 전투 시작 또는 피격 시 표시
	SetVisibility(ESlateVisibility::Hidden);
}

void UGSGASBossHpBar::SetBossName(const FText& InName)
{
	if (BossNameText)
	{
		BossNameText->SetText(InName);
	}
}

void UGSGASBossHpBar::OnCombatTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount > 0)
	{
		SetVisibility(ESlateVisibility::HitTestInvisible);
		GSGAS_LOG(LogGSGAS, Log, TEXT("GSGASBossHpBar: Combat started, HpBar shown."));
	}
	else
	{
		SetVisibility(ESlateVisibility::Hidden);
		GSGAS_LOG(LogGSGAS, Log, TEXT("GSGASBossHpBar: Combat ended, HpBar hidden."));
	}
}

void UGSGASBossHpBar::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	UAbilitySystemComponent* InASC = GetAbilitySystemComponent();
	if (!InASC) return;

	const UGSAttributeSet* AttributeSet = InASC->GetSet<UGSAttributeSet>();
	if (!AttributeSet) return;

	if (HealthBarWidget)
	{
		const float MaxHealth = AttributeSet->GetMaxHealth();
		HealthBarWidget->SetRatio(MaxHealth > 0.f ? Data.NewValue / MaxHealth : 0.f);
	}

	// 피격 시 (HP 감소) HpBar 표시
	if (Data.NewValue < Data.OldValue)
	{
		SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}
