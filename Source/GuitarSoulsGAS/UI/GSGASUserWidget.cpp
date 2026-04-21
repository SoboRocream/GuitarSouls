// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GSGASUserWidget.h"
#include "AbilitySystemBlueprintLibrary.h"

void UGSGASUserWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	if (IsValid(InOwner))
	{
		ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner);
	}
}

class UAbilitySystemComponent* UGSGASUserWidget::GetAbilitySystemComponent() const
{
	return ASC;
}
