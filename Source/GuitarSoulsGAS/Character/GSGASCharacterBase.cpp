// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GSGASCharacterBase.h"


AGSGASCharacterBase::AGSGASCharacterBase()
{
	ASC = nullptr;
}

UAbilitySystemComponent* AGSGASCharacterBase::GetAbilitySystemComponent() const
{
	return ASC;
}

void AGSGASCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}
