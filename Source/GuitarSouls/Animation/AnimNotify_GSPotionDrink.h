// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_GSPotionDrink.generated.h"

/**
 * 
 */
UCLASS(meta=(DisplayName="Potion Drink"))
class GUITARSOULS_API UAnimNotify_GSPotionDrink : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
