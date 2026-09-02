// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GSGASPlayerHUDWidget.h"
#include "AbilitySystemComponent.h"
#include "Attribute/GSAttributeSet.h"
#include "UI/GSGASBarWidget.h"
#include "UI/GSGASPotionWidget.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UGSGASPlayerHUDWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);
	UAbilitySystemComponent* InASC = GetAbilitySystemComponent();
	if (!InASC) return;

	const UGSAttributeSet* AttributeSet = InASC->GetSet<UGSAttributeSet>();
	if (!AttributeSet) return;

	HpBarWidget->SetColor(FLinearColor::Red);
	StaminaBarWidget->SetColor(FLinearColor::Green);

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

void UGSGASPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (InteractPromptText)
	{
		InteractPromptText->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UGSGASPlayerHUDWidget::ShowPromptText(const FText& InText, float Duration)
{
	// BindWidgetOptional — WBP에 TextBlock을 두지 않았다면 조용히 무시한다.
	if (!InteractPromptText) return;

	InteractPromptText->SetText(InText);
	InteractPromptText->SetVisibility(ESlateVisibility::HitTestInvisible);

	if (UWorld* World = GetWorld())
	{
		// 이전 자동 숨김 예약이 남아있으면 취소 — 새 프롬프트가 조기에 지워지는 것 방지
		World->GetTimerManager().ClearTimer(PromptTimerHandle);

		if (Duration > 0.f)
		{
			World->GetTimerManager().SetTimer(PromptTimerHandle, this, &UGSGASPlayerHUDWidget::HidePromptText, Duration, false);
		}
	}
}

void UGSGASPlayerHUDWidget::HidePromptText()
{
	if (!InteractPromptText) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(PromptTimerHandle);
	}

	InteractPromptText->SetVisibility(ESlateVisibility::Collapsed);
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
