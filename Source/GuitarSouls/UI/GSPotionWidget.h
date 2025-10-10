// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "UI/GSUserWidget.h"
#include "GSPotionWidget.generated.h"

/**
 * 
 */
UCLASS()
class GUITARSOULS_API UGSPotionWidget : public UGSUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	UTextBlock* PotionQuantityText;

public:
	void SetPotionQuantity(const int InAmount) const;
};
