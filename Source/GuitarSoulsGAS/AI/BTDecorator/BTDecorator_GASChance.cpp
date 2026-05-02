// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTDecorator/BTDecorator_GASChance.h"

UBTDecorator_GASChance::UBTDecorator_GASChance()
{
	NodeName = TEXT("GAS Chance");
}

bool UBTDecorator_GASChance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	return FMath::RandRange(0.f, 100.f) <= ChanceRate;
}

FString UBTDecorator_GASChance::GetStaticDescription() const
{
	return FString::Printf(TEXT("Chance: %.1f%%"), ChanceRate);
}
