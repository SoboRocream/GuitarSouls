// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GSGASBarWidget.h"

#include "Components/ProgressBar.h"

void UGSGASBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UGSGASBarWidget::UpdateBar(float CurrentValue, float MaxValue)
{
	const float Percent = (MaxValue > 0.f) ? (CurrentValue / MaxValue) : 0.f;
	SetRatio(Percent);
}

void UGSGASBarWidget::SetRatio(float Percent)
{
	if (StatBar)
	{
		StatBar->SetPercent(FMath::Clamp(Percent, 0.f, 1.f));
	}
}
