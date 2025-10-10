// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GSGASPlayerState.h"
#include "AbilitySystemComponent.h"

AGSGASPlayerState::AGSGASPlayerState()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
}

UAbilitySystemComponent* AGSGASPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}

