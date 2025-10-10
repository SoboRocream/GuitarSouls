// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GSBossHpBar.h"

#include "GSStatBarWidget.h"

void UGSBossHpBar::Init(UGSAttributeComponent* AttributeComponent)
{
	if (AttributeComponent)
	{
		//Delegate
		AttributeComponent->OnAttributeChanged.AddUObject(this, &UGSBossHpBar::OnAttributeChanged);
		AttributeComponent->BroadCastAttributeChanged(EGSEnumDefine::Health);
	}
}

void UGSBossHpBar::OnAttributeChanged(EGSEnumDefine AttributeType, float InValue)
{
	if (AttributeType == EGSEnumDefine::Health)
	{
		HealthBarWidget->SetRatio(InValue);
	}
}
