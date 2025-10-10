// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/GSWeapon.h"
#include "GSFistWeapon.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULS_API AGSFistWeapon : public AGSWeapon
{
	GENERATED_BODY()

public:
	AGSFistWeapon();

	virtual void EquipItem() override;
	
};
