// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GSGASBarWidget.h"
#include "Components/ProgressBar.h"

void UGSGASBarWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	Super::SetAbilitySystemComponent(InOwner);

	if (ASC)
	{
		if (StatBar)
		{
			StatBar->SetFillColorAndOpacity(DefaultFillColor);
		}
	}
}

void UGSGASBarWidget::SetRatio(float Percent)
{
	StatBar->SetPercent(Percent);
}

void UGSGASBarWidget::SetColor(FLinearColor InColor)
{
	StatBar->SetFillColorAndOpacity(InColor);
}
