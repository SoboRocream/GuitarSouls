// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator/BTDecorator_GASCheckAttribute.h"
#include "AIController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

UBTDecorator_GASCheckAttribute::UBTDecorator_GASCheckAttribute()
{
	NodeName = TEXT("GAS Check Attribute");
}

FString UBTDecorator_GASCheckAttribute::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s %s %.1f"),
		*Attribute.GetName(),
		bCheckLessThan ? TEXT("<") : TEXT(">"),
		CompareValue);
}

bool UBTDecorator_GASCheckAttribute::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory) const
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Pawn)
	{
		return false;
	}

	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn);
	if (!ASC)
	{
		return false;
	}

	bool bFound = false;
	const float CurrentValue = ASC->GetGameplayAttributeValue(Attribute, bFound);
	if (!bFound)
	{
		return false;
	}

	return bCheckLessThan ? CurrentValue < CompareValue : CurrentValue > CompareValue;

}
