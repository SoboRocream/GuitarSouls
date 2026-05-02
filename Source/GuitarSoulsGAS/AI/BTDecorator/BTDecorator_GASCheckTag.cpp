// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator/BTDecorator_GASCheckTag.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AIController.h"

UBTDecorator_GASCheckTag::UBTDecorator_GASCheckTag()
{
	NodeName = TEXT("GAS Check Tag");
}

FString UBTDecorator_GASCheckTag::GetStaticDescription() const
{
	return Super::GetStaticDescription();
}

bool UBTDecorator_GASCheckTag::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Pawn)
	{
		return false;
	}

	const UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Pawn);
	if (!ASC)
	{
		return false;
	}

	return ASC->HasMatchingGameplayTag(RequiredTag);
}
