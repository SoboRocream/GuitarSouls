// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GSGASUserWidget.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GuitarSoulsGAS.h"

void UGSGASUserWidget::SetAbilitySystemComponent(AActor* InOwner)
{
	if (IsValid(InOwner))
	{
		ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InOwner);
		if (!ASC)
		{
			GSGAS_LOG(LogGSGAS, Error, TEXT("ASC is Not Found"));
		}
	}
}

class UAbilitySystemComponent* UGSGASUserWidget::GetAbilitySystemComponent() const
{
	return ASC;
}
