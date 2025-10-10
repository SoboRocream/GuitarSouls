// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GSEnumDefine.h"
#include "AnimNotify_GSEquipWeapon.generated.h"

/**
 * 
 */
UCLASS(meta=(DisplayName="Equip Weapon"))
class GUITARSOULS_API UAnimNotify_GSEquipWeapon : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag MontageActionTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGSEquipmentType EquipmentType = EGSEquipmentType::Weapon;

public:
	UAnimNotify_GSEquipWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
