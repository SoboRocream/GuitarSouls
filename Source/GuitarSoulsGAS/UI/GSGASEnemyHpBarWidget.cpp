// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GSGASEnemyHpBarWidget.h"

#include "AbilitySystemComponent.h"
#include "Attribute/GSAttributeSet.h"

void UGSGASEnemyHpBarWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);
	if (ASC)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UGSAttributeSet::GetHealthAttribute()).AddUObject(this, &UGSGASEnemyHpBarWidget::OnHealthChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(UGSAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UGSGASEnemyHpBarWidget::OnMaxHealthChanged);
		SetColor(FLinearColor::Red);

		const UGSAttributeSet* CurrentAttributeSet = ASC->GetSet<UGSAttributeSet>();
		if (CurrentAttributeSet)
		{
			CurrentHealth = CurrentAttributeSet->GetHealth();
			CurrentMaxHealth = CurrentAttributeSet->GetMaxHealth();

			if (CurrentMaxHealth > 0.0f)
			{
				UpdateHpBar();
			}
		}
	}
}

void UGSGASEnemyHpBarWidget::OnHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentHealth = ChangeData.NewValue;
	UpdateHpBar();
}

void UGSGASEnemyHpBarWidget::OnMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	CurrentMaxHealth = ChangeData.NewValue;
	UpdateHpBar();
}

void UGSGASEnemyHpBarWidget::UpdateHpBar()
{
	SetRatio(CurrentHealth / CurrentMaxHealth);
}
