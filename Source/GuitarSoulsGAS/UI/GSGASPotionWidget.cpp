// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/GSGASPotionWidget.h"
#include "Components/TextBlock.h"

void UGSGASPotionWidget::SetPotionQuantity(int32 InAmount) const
{
	if (PotionQuantityText)
	{
		PotionQuantityText->SetText(FText::AsNumber(InAmount));
	}
}
