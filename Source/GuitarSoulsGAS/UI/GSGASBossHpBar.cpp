// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GSGASBossHpBar.h"
#include "AbilitySystemComponent.h"
#include "GSGASBarWidget.h"
#include "Attribute/GSAttributeSet.h"

void UGSGASBossHpBar::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);
	UAbilitySystemComponent* InASC = GetAbilitySystemComponent();
	if (!InASC) return;

	const UGSAttributeSet* AttributeSet = InASC->GetSet<UGSAttributeSet>();
	if (!AttributeSet) return;

	InASC->GetGameplayAttributeValueChangeDelegate(UGSAttributeSet::GetHealthAttribute()).AddUObject(this, &UGSGASBossHpBar::OnHealthChanged);

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
