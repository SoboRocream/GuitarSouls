// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/GSGASWeaponData.h"

const FGSGASAttackData* UGSGASWeaponData::GetAttackData(const FGameplayTag& AttackTag) const
{
	return AttackDataMap.Find(AttackTag);
}
