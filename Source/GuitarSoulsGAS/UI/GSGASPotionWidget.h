// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/GSGASUserWidget.h"
#include "GSGASPotionWidget.generated.h"

UCLASS()
class GUITARSOULSGAS_API UGSGASPotionWidget : public UGSGASUserWidget
{
	GENERATED_BODY()

public:
	void SetPotionQuantity(int32 InAmount) const;

protected:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite)
	TObjectPtr<class UTextBlock> PotionQuantityText;
};
