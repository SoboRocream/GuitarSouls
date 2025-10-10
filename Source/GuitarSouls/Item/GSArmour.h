// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GSEnumDefine.h"
#include "Item/GSEquipment.h"
#include "GSArmour.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULS_API AGSArmour : public AGSEquipment
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Armour")
	EGSArmourType ArmourType = EGSArmourType::Chest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment | Armour")
	float DefenseAmount = 0.f;

public:
	AGSArmour();

	virtual void EquipItem() override;
	virtual void UnEquipItem() override;
	virtual void AttachToOwner(FName SocketName) override;
	FORCEINLINE EGSArmourType GetArmourType() const { return ArmourType; }
	FORCEINLINE float GetDefenseAmount() const { return DefenseAmount; }
};
