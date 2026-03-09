// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GSGASBossHpBar.h"
#include "AbilitySystemComponent.h"
#include "Attribute/GSAttributeSet.h"
#include "UI/GSStatBarWidget.h"

void UGSGASBossHpBar::InitializeWidget(UAbilitySystemComponent* InASC)
{
	const UGSAttributeSet* AttributeSet = InASC->GetSet<UGSAttributeSet>();
	if (!AttributeSet) return;

	// ── Health 바인딩 ──────────────────────────────────────────────────────────
	InASC->GetGameplayAttributeValueChangeDelegate(
		UGSAttributeSet::GetHealthAttribute())
		.AddUObject(this, &UGSGASBossHpBar::OnHealthChanged);

	// 초기값 설정
	if (HealthBarWidget)
	{
		HealthBarWidget->SetRatio(AttributeSet->GetHealth() / AttributeSet->GetMaxHealth());
	}
}

void UGSGASBossHpBar::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (!HealthBarWidget) return;

	const UGSAttributeSet* AttributeSet = GetAbilitySystemComponent()->GetSet<UGSAttributeSet>();
	if (!AttributeSet) return;

	HealthBarWidget->SetRatio(Data.NewValue / AttributeSet->GetMaxHealth());
}
