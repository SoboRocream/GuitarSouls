// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GSGASUserWidget.h"
#include "AbilitySystemComponent.h"

void UGSGASUserWidget::SetASC(class UAbilitySystemComponent* InASC)
{
	if (!InASC) return;

	ASC = InASC;
	InitializeWidget(InASC);
}
