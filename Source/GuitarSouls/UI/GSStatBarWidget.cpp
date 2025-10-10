// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GSStatBarWidget.h"

#include "Components/ProgressBar.h"

UGSStatBarWidget::UGSStatBarWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

void UGSStatBarWidget::SetRatio(float Ratio) const
{
	if (StatBar)
	{
		StatBar->SetPercent(Ratio);
	}
}

void UGSStatBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StatBar)
	{
		StatBar->SetFillColorAndOpacity(FillColorAndOpacity);
	}
}
