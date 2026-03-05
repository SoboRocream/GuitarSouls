// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GSGASPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Attribute/GSAttributeSet.h"

AGSGASPlayerState::AGSGASPlayerState()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));

	AttributeSet = CreateDefaultSubobject<UGSAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AGSGASPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}

