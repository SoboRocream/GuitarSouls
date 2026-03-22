// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/GSGASWeaponData.h"
#include "GuitarSoulsGAS.h"

UAnimMontage* UGSGASWeaponData::GetMontageForTag(const FGameplayTag& Tag) const
{
	if (const TObjectPtr<UAnimMontage>* Found = MontageMap.Find(Tag))
	{
		return Found->Get();
	}
 
	GSGAS_LOG(LogGSGAS, Warning, TEXT("Montage not found for tag: %s"), *Tag.ToString());
	return nullptr;
}

float UGSGASWeaponData::GetStaminaCost(const FGameplayTag& Tag) const
{
	if (const float* Found = StaminaCostMap.Find(Tag))
	{
		return *Found;
	}
 
	return 0.f;
}

float UGSGASWeaponData::GetFinalDamage(const FGameplayTag& Tag) const
{
	if (const float* Multiplier = DamageMultiplierMap.Find(Tag))
	{
		return BaseDamage * (*Multiplier);
	}
 
	return BaseDamage;
}
