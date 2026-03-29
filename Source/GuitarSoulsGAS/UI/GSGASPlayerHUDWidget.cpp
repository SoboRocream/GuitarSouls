// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GSGASPlayerHUDWidget.h"
#include "AbilitySystemComponent.h"
#include "Attribute/GSAttributeSet.h"
#include "UI/GSStatBarWidget.h"
#include "UI/GSPotionWidget.h"
#include "Player/GSGASPlayerState.h"

void UGSGASPlayerHUDWidget::ShowInteractUI(const FText& InteractText)
{
}

void UGSGASPlayerHUDWidget::HideInteractUI()
{
}

void UGSGASPlayerHUDWidget::InitializeWidget(UAbilitySystemComponent* InASC)
{
	const UGSAttributeSet* AttributeSet = InASC->GetSet<UGSAttributeSet>();
	if (!AttributeSet) return;

	// ── Health 바인딩 ──────────────────────────────────────────────────────────
	InASC->GetGameplayAttributeValueChangeDelegate(
		UGSAttributeSet::GetHealthAttribute())
		.AddUObject(this, &UGSGASPlayerHUDWidget::OnHealthChanged);

	if (HpBarWidget)
	{
		HpBarWidget->SetRatio(AttributeSet->GetHealth() / AttributeSet->GetMaxHealth());
	}

	// ── Stamina 바인딩 ────────────────────────────────────────────────────────
	InASC->GetGameplayAttributeValueChangeDelegate(
		UGSAttributeSet::GetStaminaAttribute())
		.AddUObject(this, &UGSGASPlayerHUDWidget::OnStaminaChanged);

	if (StaminaBarWidget)
	{
		StaminaBarWidget->SetRatio(AttributeSet->GetStamina() / AttributeSet->GetMaxStamina());
	}

	// ── PotionCount 바인딩 ────────────────────────────────────────────────────
	InASC->GetGameplayAttributeValueChangeDelegate(
		UGSAttributeSet::GetPotionCountAttribute())
		.AddUObject(this, &UGSGASPlayerHUDWidget::OnPotionCountChanged);

	if (PotionWidget)
	{
		PotionWidget->SetPotionQuantity(FMath::FloorToInt(AttributeSet->GetPotionCount()));
	}
}

void UGSGASPlayerHUDWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (!HpBarWidget) return;

	const UGSAttributeSet* AttributeSet = GetAbilitySystemComponent()->GetSet<UGSAttributeSet>();
	if (!AttributeSet) return;

	HpBarWidget->SetRatio(Data.NewValue / AttributeSet->GetMaxHealth());
}

void UGSGASPlayerHUDWidget::OnStaminaChanged(const FOnAttributeChangeData& Data)
{
	if (!StaminaBarWidget) return;

	const UGSAttributeSet* AttributeSet = GetAbilitySystemComponent()->GetSet<UGSAttributeSet>();
	if (!AttributeSet) return;

	StaminaBarWidget->SetRatio(Data.NewValue / AttributeSet->GetMaxStamina());
}

void UGSGASPlayerHUDWidget::OnPotionCountChanged(const FOnAttributeChangeData& Data)
{
	if (!PotionWidget) return;

	PotionWidget->SetPotionQuantity(FMath::FloorToInt(Data.NewValue));
}
