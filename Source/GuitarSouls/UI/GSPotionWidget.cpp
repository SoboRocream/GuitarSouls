// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GSPotionWidget.h"

#include "Kismet/KismetTextLibrary.h"

void UGSPotionWidget::SetPotionQuantity(const int InAmount) const
{
	PotionQuantityText->SetText(UKismetTextLibrary::Conv_IntToText(InAmount));
}
